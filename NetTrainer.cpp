#include "NetTrainer.hpp"
#include <random>
#include "Chess.hpp"
#include <fstream>

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
    for (int i = train_threads; i < train_threads + test_threads; i++) {
        this->threads[i] = std::make_shared<std::thread>(&NetTrainer::testLoop, this);
    }

    while(1) {
        auto lock = std::unique_lock<std::mutex>(this->data_mutex);
        this->data_cv.wait(lock, [&]() {
            return this->new_samples >= this->target_samples;
        });

        //reusing data a bit
        this->trainEpoch();
        new_samples = 0;

        if(this->data.size() >= 100000)
            this->data.erase(this->data.begin(), this->data.end() - 100000);
    }
}

void NetTrainer::dataGenLoop() {
    torch::set_num_threads(1);
    torch::NoGradGuard no_grad;
    while(1) {
        
        // std::cout << "starting next game" << std::endl;

        NN_MCTS mcts(this->baseGame, this->runner, this->interface);

        std::vector<sample> samples;

        int moves_made = 0;
        mcts.setRandomness(true);

        while (!mcts.getRoot()->getTerminal()){
            // std::cout << "moves made: " << moves_made << std::endl;
            if(moves_made > 25)
                mcts.setRandomness(false);
            mcts.run(500);
            
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

        int game_result = mcts.getRoot()->getGame()->gameStatus(mcts.getRoot()->getPossibleMoves());
        
        double result = 0;

        if(game_result == 1 || game_result == 2){
            result = 0.5;
        } else {
            result = mcts.getRoot()->getGame()->getCurrentPlayer();
        }

        if(moves_made == 500) {
            result = 0.5;
        }

        auto lock = std::unique_lock<std::mutex>(this->data_mutex);
        for (auto& s : samples) {
            float res = result;
            s.result = torch::from_blob(&res, {1, 1}, torch::kFloat).clone();
            this->data.push_back(s);
        }
        this->new_samples += samples.size();

        std::cout << "result " << result << " data size: " << this->data.size() << std::endl;
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

            auto mask = torch::cat({zero_mask, full_mask}, 1);
            
            masks.push_back(mask);
            auto filler = torch::zeros({1, longest - current_size}, torch::kLong);
            moves[j] = torch::cat({moves[j], filler}, 1);
            move_scores[j] = torch::cat({move_scores[j], filler.to(torch::kFloat32)}, 1);
        }
        auto boards_tensor = torch::cat(boards, 0).to(*(this->device));
        auto moves_tensor = torch::cat(moves, 0).to(*(this->device));
        auto move_scores_tensor = torch::cat(move_scores, 0).to(*(this->device));
        auto results_tensor = torch::cat(results, 0).to(*(this->device));
        auto masks_tensor = torch::cat(masks, 0).to(*(this->device));

        this->optimizer->zero_grad();
        auto loss = this->net->loss(boards_tensor, moves_tensor, masks_tensor, move_scores_tensor, results_tensor);
        avg_loss += loss.item<float>();
        iters++;
        loss.backward();
        this->optimizer->step();

    }
    std::ofstream logfile;
    logfile.open("losslog.txt", std::ios_base::app);
    logfile << avg_loss/iters << std::endl;
    logfile.close();
    std::cout << "avg loss: " << avg_loss/iters << std::endl;
}

void NetTrainer::testLoop() {
    torch::set_num_threads(1);
    while(1) {
        std::shared_ptr<Game> real_game = this->baseGame->clone();
        NN_MCTS mcts(this->baseGame, this->runner, this->interface);
        MCTS other(this->baseGame);

        auto moves = real_game->getPossibleMoves();

        int player = 0;
        int moves_made = 0;
        while (real_game->gameStatus(moves) == -1)
        {
            std::shared_ptr<Move> move = nullptr;

            if (player == 0)
            {
                mcts.run(500);
                // auto scores = mcts.getRoot()->getMoveScores();
                // std::sort(scores.begin(), scores.end());
                // for(int i = 0; i<scores.size(); i++){
                //     std::cout << scores[i] << " ";
                // }
                // std::cout << std::endl;

                double w = mcts.getRoot()->getScore(player);
                double n = mcts.getRoot()->getSimulations();
                std::cout << "neural board value: " << w / n << std::endl;
                auto best_move = mcts.getBestMove();
                move = best_move.first;
            }
            else
            {   
                other.run(1000);
                double w = other.getRoot()->getScore(player);
                double n = other.getRoot()->getSimulations();
                std::cout << "random board value: " << w / n << std::endl;
                auto best_move = other.getBestMove();
                move = best_move.first;
            }

            other.doMove(move);
            mcts.doMove(move);
            real_game->simulateMove(move);
            player = 1 - player;
            real_game->setCurrentPlayer(player);
            std::cout << real_game->printBoard() << std::endl;
            moves = real_game->getPossibleMoves();
            moves_made++;
            if(moves_made == 500) break;
        }

        std::ofstream logfile;
        logfile.open("winlog.txt", std::ios_base::app);
        

        switch (real_game->gameStatus(moves))
        {
        case GameStatus::DRAW:
            std::cout << "Draw" << std::endl;
            logfile << 0.5 << std::endl;
            break;
        case GameStatus::STALE_MATE:
            std::cout << "Stalemate" << std::endl;
            logfile << 0.5 << std::endl;
            break;
        case GameStatus::CHECK_MATE:
            std::cout << "Checkmate, ";
            if (!player)
            {
                std::cout << "Black wins" << std::endl;
                logfile << 0.0 << std::endl;
            }
            else
            {
                std::cout << "White wins" << std::endl;
                logfile << 1.0 << std::endl;
            }
            break;
        default:
            std::cout << "Timeout" << std::endl;
            logfile << 0.5 << std::endl;
            break;
        }

        logfile.close();
    }
}

// void NetTrainer::shutdown() {
//     this->runner->shutdown();
//     for (auto &t : this->threads) {
//         t->join();
//     }
// }