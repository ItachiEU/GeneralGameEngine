#include "Chess.hpp"
// #include "NN_MCTS.hpp"
// #include "NetRunner.hpp"
// #include "ChessNN.hpp"
// #include "NetTrainer.hpp"

int main()
{
   srand(time(NULL));

   std::shared_ptr<Chess> real_game = std::make_shared<Chess>();
   std::string move;
   auto moves = real_game->getPossibleMoves();
   int count = 0;
   while (count < 10)
   {
      for (auto move : moves)
      {
         std::cout << real_game->LANfromMove(std::static_pointer_cast<ChessMove>(move)) << std::endl;
      }
      auto mv = moves[rand() % moves.size()];
      std::cout << real_game->LANfromMove(std::static_pointer_cast<ChessMove>(mv)) << std::endl;
      real_game->simulateMove(mv);
      std::cout << real_game->printBoard() << std::endl;
      real_game->setCurrentPlayer(1 - real_game->getCurrentPlayer());
      moves = real_game->getPossibleMoves();
      count++;
   }
   // while (count < 10)
   // {
   //    std::string input;
   //    std::cin >> input;
   //    auto mv = real_game->moveFromLANNotation(input);
   //    std::cout << mv->getFromRow() << " " << mv->getFromCol() << " " << mv->getToRow() << " " << mv->getToCol() << " " << mv->getPiece() << " " << mv->getColor() << std::endl;
   //    real_game->simulateMove(mv);
   //    std::cout << real_game->printBoard() << std::endl;
   //    real_game->setCurrentPlayer(1 - real_game->getCurrentPlayer());
   //    count++;
   // }
   // std::shared_ptr<Game> game = std::make_shared<Chess>();

   // auto net = std::static_pointer_cast<Net>(std::make_shared<ChessNet>(64, 3));
   // net->to(torch::kCUDA);
   // auto device = std::make_shared<torch::Device>(torch::kCUDA);
   // auto runner = std::make_shared<NetRunner>(net, device, 1);
   // auto opt = std::make_shared<torch::optim::Adam>(net->parameters(), 0.0003);
   // auto interface = std::make_shared<ChessNNInterface>();

   // auto trainer = std::make_shared<NetTrainer>(game, interface, net, device, opt, 1, 64);

   // trainer->train(10000, 0, 1, 4);

   return 0;
}