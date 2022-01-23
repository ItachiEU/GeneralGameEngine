#include "NetTrainer.hpp"
#include <random>
#include "Chess.hpp"

NetTrainer::NetTrainer(
    std::shared_ptr<Game> game,
    std::shared_ptr<NN_Interface> interface,
    std::shared_ptr<Net> net,
    std::shared_ptr<torch::Device> device,
    std::shared_ptr<torch::optim::Adam> optimizer,
    int eval_batch_size,
    int train_batch_size
) {
    this->baseGame = game;
    this->runner = std::make_shared<NetRunner>(net, device, eval_batch_size);
    this->interface = interface;
    this->net = net;
    this->optimizer = optimizer;
    this->device = device;
    this->train_batch_size = train_batch_size;
}

void NetTrainer::train(int target_samples, int train_threads, int test_threads) {
    this->target_samples = target_samples;
    this->threads.resize(train_threads + test_threads);
    for (int i = 0; i < train_threads; i++) {
        this->threads[i] = std::make_shared<std::thread>(&NetTrainer::dataGenLoop, this);
    }
    // for (int i = train_threads; i < train_threads + test_threads; i++) {
    //     this->threads[i] = std::make_shared<std::thread>(&NetTrainer::testLoop, this);
    // }

    while(1) {
        auto lock = std::unique_lock<std::mutex>(this->data_mutex);
        this->data_cv.wait(lock, [&]() {
            return (int)this->data.size() >= this->train_batch_size;
        });

        this->trainEpoch();

        this->data.clear();
    }
}

void NetTrainer::dataGenLoop() {
    torch::NoGradGuard no_grad;
    while(1) {
        
        std::cout << "starting next game" << std::endl;

        NN_MCTS mcts(this->baseGame, this->runner, this->interface);

        std::vector<sample> samples;

        int moves_made = 0;
        mcts.setRandomness(true);

        while (!mcts.getRoot()->getTerminal()){
            if(moves_made > 25)
                mcts.setRandomness(false);
            mcts.run(3);
            
            auto game = mcts.getRoot()->getGame();
            auto input = interface->getNNInput(game, game->getCurrentPlayer());
            auto moves = game->getPossibleMoves();
            auto moves_vec = interface->movesRepr(moves);

            std::vector<float> move_visits;
            int s = mcts.getRoot()->getPossibleMoves().size();
            double n = mcts.getRoot()->getSimulations();
            for (int i = 0; i < s; i++) {
                if(mcts.getRoot()->getChildren().find(i) != mcts.getRoot()->getChildren().end()) {
                    move_visits.push_back((double)mcts.getRoot()->getChildren().at(i)->getSimulations()/n);
                } else {
                    move_visits.push_back(0);
                }
            }
            // std::cout << mcts.getRoot()->getGame()->printBoard() << std::endl;
            auto move = mcts.getBestMove();
            mcts.doMove(move.first);
            moves_made++;

            sample sa;
            sa.board = input;
            sa.moves = moves_vec;
            sa.moveScores = torch::from_blob(move_visits.data(), {1,s}, torch::kFloat).clone();

            samples.push_back(sa);
            if(moves_made == 500) break;
        }

        double result = mcts.getRoot()->getScore(0);
        if(moves_made == 500) {
            result = 0.5;
        }

        auto lock = std::unique_lock<std::mutex>(this->data_mutex);
        for (auto& s : samples) {
            s.result = torch::from_blob(&result, {1, 1}, torch::kFloat).clone();
            this->data.push_back(s);
        }

        this->data_cv.notify_all();
        lock.unlock();
    }
}

void NetTrainer::trainEpoch(){
    std::random_shuffle(this->data.begin(), this->data.end());

    int s = this->data.size();
    std::cout << "data size: "<< s << std::endl;
    double avg_loss = 0;
    int iters = 0;
    for(int i = 0; i < s; i += this->train_batch_size){
        int batch = i;
        int batch_end = std::min(i + this->train_batch_size, s);
        int batch_size = batch_end - batch;

        std::vector<torch::Tensor> boards;
        std::vector<torch::Tensor> moves;
        std::vector<torch::Tensor> move_scores;
        std::vector<torch::Tensor> results;
        std::vector<torch::Tensor> masks;

        int longest = 0;
        for (int j = batch; j != batch_end; j++) {
            auto& d= this->data[j];
            boards.push_back(d.board);
            moves.push_back(d.moves);
            longest = std::max(longest, (int)d.moves.size(1));
            move_scores.push_back(d.moveScores);
            results.push_back(d.result);
        }

        for (int j = 0; j != batch_size; j++) {
            int current_size = moves[j].size(1);
            auto zero_mask = torch::zeros({1, current_size}, torch::kFloat);
            auto full_mask = torch::ones({1, longest - current_size}, torch::kFloat)*-INFINITY;
            
            // std:: cout << "longest: " << longest << std::endl;
            // std:: cout << "current: " << current_size << std::endl;
            // std:: cout << "mask: " << zero_mask.sizes << std::endl;
            // std:: cout << "mask: " << zero_mask.sizes << std::endl;

            auto mask = torch::cat({zero_mask, full_mask}, 1);
            
            masks.push_back(mask);
            auto filler = torch::zeros({1, longest - current_size}, torch::kLong);
            moves[j] = torch::cat({moves[j], filler}, 1);
            move_scores[j] = torch::cat({move_scores[j], filler.to(torch::kFloat32)}, 1);
        }
        auto boards_tensor = torch::cat(boards, 0).to(*(this->device));
        auto moves_tensor = torch::cat(moves, 0);
        auto move_scores_tensor = torch::cat(move_scores, 0);
        auto results_tensor = torch::cat(results, 0);
        auto masks_tensor = torch::cat(masks, 0);

        // std::cout << boards_tensor.sizes() << std::endl;
        // std::cout << moves_tensor.sizes() << std::endl;
        // std::cout << move_scores_tensor.sizes() << std::endl;
        // std::cout << results_tensor.sizes() << std::endl;
        // std::cout << masks_tensor.sizes() << std::endl;
        

        this->optimizer->zero_grad();
        auto loss = this->net->loss(boards_tensor, moves_tensor, masks_tensor, move_scores_tensor, results_tensor);
        avg_loss += loss.item<float>();
        iters++;
        loss.backward();
        this->optimizer->step();
    }
    std::cout << "avg loss: " << avg_loss/iters << std::endl;
}

void NetTrainer::testLoop() {
    while(1) {
        std::shared_ptr<Game> real_game = this->baseGame->clone();
        NN_MCTS mcts(this->baseGame, this->runner, this->interface);

        auto moves = real_game->getPossibleMoves();

        int player = 0;

        while (real_game->gameStatus(moves) == -1)
        {
            std::shared_ptr<Move> move = nullptr;

            if (player == 0)
            {
                mcts.run();
                double w = mcts.getRoot()->getScore(player);
                double n = mcts.getRoot()->getSimulations();
                std::cout << "board value: " << w / n << std::endl;
                auto best_move = mcts.getBestMove();
                move = best_move.first;
            }
            else
            {
                move = real_game->getPossibleMoves()[rand() % real_game->getPossibleMoves().size()];
            }

            mcts.doMove(move);
            real_game->simulateMove(move);
            player = 1 - player;
            real_game->setCurrentPlayer(player);
            std::cout << real_game->printBoard() << std::endl;
            moves = real_game->getPossibleMoves();
        }

        switch (real_game->gameStatus(moves))
        {
        case GameStatus::DRAW:
            std::cout << "Draw" << std::endl;
            break;
        case GameStatus::STALE_MATE:
            std::cout << "Stalemate" << std::endl;
            break;
        case GameStatus::CHECK_MATE:
            std::cout << "Checkmate, ";
            if (!player)
            {
                std::cout << "Black wins" << std::endl;
            }
            else
            {
                std::cout << "White wins" << std::endl;
            }
            break;
        default:
            break;
        }
    }
}

// void NetTrainer::shutdown() {
//     this->runner->shutdown();
//     for (auto &t : this->threads) {
//         t->join();
//     }
// }