#include "Chess.hpp"
#include "MCTS.hpp"

int main()
{
   srand(time(NULL));
   std::shared_ptr<Game> real_game = std::make_shared<Chess>();
   std::shared_ptr<Game> game = std::make_shared<Chess>();
   
   MCTS mcts(game);

   auto moves = real_game->getPossibleMoves();

   int player = 0;

   while(real_game->gameStatus(moves) == GameStatus::IN_PROGRESS)
   {
      std::shared_ptr<Move> move = nullptr;

      if(player == 0){
         mcts.run();
         auto best_move = mcts.getBestMove();
         move = best_move.first;
      } else {
         move = real_game->getPossibleMoves()[rand() % real_game->getPossibleMoves().size()];
      }

      mcts.doMove(move);
      real_game->simulateMove(move);
      std::cout << real_game->printBoard() << std::endl;
      moves = real_game->getPossibleMoves();
      player = 1 - player;
      real_game->setCurrentPlayer(player);
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
         if(player) {
            std::cout << "Black wins" << std::endl;
         } else {
            std::cout << "White wins" << std::endl;
         }
         break;
      default:
         break;
   }

   return 0;
}