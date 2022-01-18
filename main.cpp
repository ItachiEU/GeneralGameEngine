#include "Chess.hpp"
#include "NN_MCTS.hpp"
#include "NetRunner.hpp"
#include "ChessNN.hpp"

int main()
{
   srand(time(NULL));


   std::shared_ptr<Game> real_game = std::make_shared<Chess>();

   std::shared_ptr<Game> game = std::make_shared<Chess>();
   
   auto net = std::static_pointer_cast<Net>(std::make_shared<ChessNet>(32, 3));
   auto device = std::make_shared<torch::Device>(torch::kCPU);
   auto runner = std::make_shared<NetRunner>(net, device, 1);
   auto interface = std::make_shared<ChessNNInterface>();

   NN_MCTS mcts(game, runner, interface);

   auto moves = real_game->getPossibleMoves();

   int player = 0;

   while (real_game->gameStatus(moves) == GameStatus::IN_PROGRESS)
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

   runner->shutdown();

   return 0;
}