#include "UCI.hpp"
#include "NetRunner.hpp"
#include "ChessNN2.hpp"
#include <fstream>

//plot the search tree as a graphviz file
std::pair<std::string, int> generate_graphviz(std::shared_ptr<Node> node, int idx){

    std::string output;

    double value = node->getScore(0) / node->getSimulations();
    
    std::string color = node->getGame()->getCurrentPlayer() == 0 ? "green" : "red"; 

    if(node->getTerminal()){
        output += "node" + std::to_string(idx) + " [label=\"" + std::to_string(value) + " | "+ std::to_string(node->getSimulations()) + "\";" + "color=" +color+"];\n";
        return std::make_pair(output, idx);
    }

    output += "node" + std::to_string(idx) + " [label=\"" + std::to_string(value) + "|"+ std::to_string(node->getSimulations()) + "\";" + "color=" +color+"];\n";

    int idc = idx;
    for(auto child : node->getChildren()){
        if(child.second->getSimulations() <= 1) continue;
        int next_node = idc + 1;
        std::pair<std::string, int> child_graphviz = generate_graphviz(child.second, next_node);
        idc = child_graphviz.second;
        output += "node" + std::to_string(idx) + " -> node" + std::to_string(next_node) + ";\n";
        output += child_graphviz.first;
    }

    return std::make_pair(output, idc);
}


void uci_loop(){

    std::ofstream log("enginelog.txt");

    auto game = std::make_shared<Chess>();

    auto net = std::static_pointer_cast<Net>(std::make_shared<ChessNet2>(128, 3));
    net->to(torch::kCUDA);
    auto device = std::make_shared<torch::Device>(torch::kCUDA);
    auto runner = std::make_shared<NetRunner>(net, device, 1);
    auto interface = std::make_shared<ChessNNInterface2>();

    torch::load(net, "model.pt");

    auto mcts = std::make_shared<NN_MCTS>(game, runner, interface);

    // mcts->run(1000);

    // std::ofstream graph("graphviz.dot");
    // graph << "digraph G {\n";

    // std::pair<std::string, int> graphviz = generate_graphviz(mcts->getRoot(), 0);
    // graph << graphviz.first;

    // graph << "}\n";
    // graph.close();
    // std::cerr << "graphviz.dot generated\n" << std::endl;


    while(true){
        std::string s;

        std::cin >> s;

        log << s << std::endl;

        if(s == "uci"){
            std::cout << "id name General Game Engine" << std::endl;
            std::cout << "id author MichalOp & ItachiEU" << std::endl;
            std::cout << "uciok" << std::endl;
            log << "uciok" << std::endl;
        }
        else if(s == "isready"){
            std::cout << "readyok" << std::endl;
            log << "readyok" << std::endl;
        }
        else if(s == "ucinewgame"){
            std::cout << "info string ucinewgame" << std::endl;
            log << "info string ucinewgame" << std::endl;
        }
        else if(s == "position"){

            std::string line = "";
            std::getline(std::cin, line);


            log << line << std::endl;
            std::stringstream ss(line);

            std::string s2;
            ss >> s2;

            if(s2 == "fen")
            {
                std::string ignore;
                ss >> ignore;
                game = std::make_shared<Chess>();
                log << "reset game" << std::endl;
            }

            if(s2 == "startpos")
            {
                game = std::make_shared<Chess>();
                log << "reset game" << std::endl;
            }

            while (s2 != "moves" && ss.good()){
                ss >> s2;
            }

            std::shared_ptr<Move> mv = nullptr;

            if(s2 == "moves")
            {
                std::string s3;
                log << "moves" << std::endl;
                while(ss >> s3)
                {
                    mv = game->moveFromLANNotation(s3);
                    game->simulateMove(mv);
                    game->setCurrentPlayer(1 - game->getCurrentPlayer());
                }
            }

            if(mv != nullptr){
                mcts->doMove(mv);
            }

        }
        else if(s == "go"){

            std::string line = "";
            std::getline(std::cin, line);


            log << line << std::endl;
            std::stringstream ss(line);

            int wtime = 0;
            int btime = 0;
            int winc = 0;
            int binc = 0;

            while(ss >> s){
                if(s == "wtime"){
                    ss >> wtime;
                }
                else if(s == "btime"){
                    ss >> btime;
                }
                else if(s == "winc"){
                    ss >> winc;
                }
                else if(s == "binc"){
                    ss >> binc;
                }
                else if(s == "movetime"){
                    ss >> winc;
                    binc = winc;
                }
            }

            log << "current player:" << game->getCurrentPlayer() << std::endl;

            if(game->getPossibleMoves().size() == 0){
                log << "No moves available!" << std::endl;
                continue;
            }

            int think_time = 0;

            if(game->getCurrentPlayer() == 0){
                think_time = wtime/20 + (winc*2/3);
            }
            else{
                think_time = btime/20 + (binc*2/3);
            }
            
            using namespace std::chrono;
            auto start = high_resolution_clock::now();
            auto end = high_resolution_clock::now();
            auto elapsed = duration_cast<milliseconds>(end - start);
            int iterations = 0;
            while (true)
            {
                mcts->run(1);
                iterations++;
                end = high_resolution_clock::now();
                elapsed = duration_cast<milliseconds>(end - start);
                if (elapsed.count() > think_time)
                {
                    break;
                }
            }
            
            log << "thinking time: " << think_time << std::endl;
            log << "actual thinking time: " << elapsed.count() << std::endl;
            log << "iterations: " << iterations << std::endl;

            auto best = mcts->getBestMove();
            auto bestMove = best.first;
            mcts->doMove(bestMove);
            
            auto root = mcts->getRoot();
            log << "value: " << best.second->getScore(game->getCurrentPlayer()) / (double)best.second->getSimulations() << std::endl;
            log << "visits: " << (double)best.second->getSimulations() << std::endl;

            std::cout << "bestmove " << game->LANfromMove(std::static_pointer_cast<ChessMove>(bestMove)) << std::endl;
            log << "bestmove " << game->LANfromMove(std::static_pointer_cast<ChessMove>(bestMove)) << std::endl;

        }
        else if(s == "quit"){
            log << "quit" << std::endl;
            return;
        }
    }
}