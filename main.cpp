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

      std::cout << real_game->getPossibleMoves().size() << std::endl;
      std::cout << mcts.getRoot()->getChildren().size() << std::endl;

      mcts.doMove(move);
      real_game->simulateMove(move);
      std::cout << mcts.getRoot()->getGame()->printBoard() << std::endl;
      std::cout << real_game->printBoard() << std::endl;

      moves = real_game->getPossibleMoves();
      player = 1 - player;
   }

   return 0;
}