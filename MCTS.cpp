#include "MCTS.hpp"

MCTS::MCTS(std::shared_ptr<Game> game)
{
   this->root = std::make_shared<Node>(game->getPossibleMoves(), game);
}

void MCTS::run()
{
   int player = this->getRoot()->getGame()->getCurrentPlayer();
   for (int i = 0; i < 1000; i++) // placeholder
   {
      std::shared_ptr<Node> node = this->treePolicy(this->getRoot());
      std::cerr << "after policy" << std::endl;
      int result = this->simulate(node);
      std::cerr << "after simulate" << std::endl;
      backpropagate(node, player, result);
      std::cerr << "after propagate" << std::endl;
   }
}

std::shared_ptr<Node> MCTS::getRoot()
{
   return this->root;
}

void MCTS::setRoot(std::shared_ptr<Node> node)
{
   this->root = node;
}

std::shared_ptr<Node> MCTS::treePolicy(std::shared_ptr<Node> node)
{
   std::cerr << "og node children size: " << node->getChildren().size() << std::endl;
   int currentPlayer = node->getGame()->getCurrentPlayer();

   std::cerr << "got player in policy" << std::endl;
   while (node->getTerminal() == false)
   {
      if (node->getLastExpanded() < (int)(node->getPossibleMoves().size() - 1))
      {
         return this->expand(node);
      }
      else
      {
         std::cerr << "node children size: " << node->getChildren().size() << std::endl;
         node = this->bestChild(node, currentPlayer);
         std::cerr << "node from best child " << (bool)(node == nullptr) << std::endl;
      }
      currentPlayer = 1 - currentPlayer;
   }
   return node;
}

std::shared_ptr<Node> MCTS::bestChild(std::shared_ptr<Node> node, int currentPlayer)
{
   double bestScore = -1.0;
   std::shared_ptr<Node> chosenChild = nullptr;
   std::cerr << node->getChildren().size() << std::endl;
   for (auto son : node->getChildren())
   {
      double score = (double)(son.second->getScore(currentPlayer) / (double)son.second->getSimulations()) + 1.4 * sqrt((2 * log(node->getSimulations()) / son.second->getSimulations()));
      std::cerr << "score: " << score << std::endl;
      if (score > bestScore)
      {
         chosenChild = son.second;
         std::cerr << "is chosen child nullptr: " << (bool)(chosenChild == nullptr) << std::endl;
         bestScore = score;
      }
   }
   return chosenChild;
}

std::shared_ptr<Node> MCTS::expand(std::shared_ptr<Node> node)
{
   node->setLastExpanded(node->getLastExpanded() + 1);
   auto gameCopy = node->getGame()->clone();
   gameCopy->simulateMove(node->getPossibleMoves()[node->getLastExpanded()]);

   auto child = std::make_shared<Node>(gameCopy->getPossibleMoves(), gameCopy);
   node->getChildren()[node->getLastExpanded()] = child;
   if (child->getGame()->gameStatus(gameCopy->getPossibleMoves()) != -1)
      child->setTerminal(true);

   return child;
}

double MCTS::simulate(std::shared_ptr<Node> node)
{
   auto root(node);
   int player = root->getGame()->getCurrentPlayer();
   auto gameCopy = root->getGame()->clone();
   std::vector<std::shared_ptr<Move>> possibleMoves = gameCopy->getPossibleMoves();
   while (gameCopy->gameStatus(possibleMoves) == -1)
   {
      std::shared_ptr<Move> move = possibleMoves[rand() % possibleMoves.size()];
      gameCopy->simulateMove(move);
      gameCopy->setCurrentPlayer(1 - gameCopy->getCurrentPlayer());
      possibleMoves = gameCopy->getPossibleMoves();
   }
   double result;
   if (gameCopy->gameStatus(possibleMoves) == 1 or gameCopy->gameStatus(possibleMoves) == 2)
      result = 0.5;
   if (gameCopy->gameStatus(possibleMoves) == 0)
      result = player != gameCopy->getCurrentPlayer() ? 1.0 : 0.0;
   return result;
}

void MCTS::backpropagate(std::shared_ptr<Node> end, int player, int result)
{
   do
   {
      if (result == 0.5)
      {
         end->setScore(0, end->getScore(0) + 0.5);
         end->setScore(1, end->getScore(1) + 0.5);
      }
      else
         end->setScore(player, result);
      end->setSimulations(end->getSimulations() + 1);

      end = end->getParent();
   } while (end != nullptr);
}
