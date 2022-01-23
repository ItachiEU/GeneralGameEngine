#include "MCTS.hpp"

MCTS::MCTS(std::shared_ptr<Game> game)
{
   auto g = game->clone(); // required because of multithreading
   this->root = std::make_shared<Node>(g->getPossibleMoves(), g);
}

void MCTS::run(int simulations)
{
   
   for (int i = 0; i < simulations; i++) // placeholder
   {
      std::shared_ptr<Node> node(this->treePolicy(this->getRoot()));

      double result = this->simulate(node);

      int player = node->getGame()->getCurrentPlayer();

      assert ((result >= 0.0) && (result <= 1.0) && "simulate returned nonsense result");

      backpropagate(node, player, result);
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
   int currentPlayer = node->getGame()->getCurrentPlayer();
   while (node->getTerminal() == false)
   {
      if (node->getLastExpanded() < (int)(node->getPossibleMoves().size() - 1))
      {
         return this->expand(node);
      }
      else
      {
         node = this->bestChild(node, currentPlayer);
      }
      currentPlayer = 1 - currentPlayer;
   }
   return node;
}

std::shared_ptr<Node> MCTS::bestChild(std::shared_ptr<Node> node, int currentPlayer)
{
   double bestScore = -1.0;
   std::shared_ptr<Node> chosenChild = nullptr;
   for (auto son : node->getChildren())
   {
      double score = (double)(son.second->getScore(currentPlayer) / (double)son.second->getSimulations()) + 1.4 * sqrt((2 * log(node->getSimulations()) / son.second->getSimulations()));
      if (score > bestScore)
      {
         chosenChild = son.second;
         bestScore = score;
      }
   }
   return chosenChild;
}

std::pair<std::shared_ptr<Move>, std::shared_ptr<Node>> MCTS::getBestMove()
{
   int best_cnt = -1;
   std::shared_ptr<Node> chosenChild = nullptr;
   std::shared_ptr<Move> chosenMove = nullptr;
   for (auto son : root->getChildren())
   {
      if(son.second->getSimulations() > best_cnt)
      {
         chosenChild = son.second;
         chosenMove = root->getPossibleMoves()[son.first];
         best_cnt = son.second->getSimulations();
      }
   }
   return make_pair(chosenMove, chosenChild);
}

void MCTS::doMove(std::shared_ptr<Move> move)
{
   int target_id = -1;
   int s = root->getPossibleMoves().size();
   for(int i = 0; i < s; i++)
   {
      if(root->getPossibleMoves()[i]->eq(move))
      {
         target_id = i;
         break;
      }
   }

   assert ((target_id != -1 && "move was not found in possible moves"));

   if(root->getChildren().find(target_id) == root->getChildren().end())
   {
      auto gameCopy = root->getGame()->clone();
      gameCopy->simulateMove(move);
      gameCopy->setCurrentPlayer(1 - gameCopy->getCurrentPlayer());
      this->setRoot(std::make_shared<Node>(gameCopy->getPossibleMoves(), gameCopy, root));
   } else{
      this->setRoot(root->getChildren()[target_id]);
   }
}

std::shared_ptr<Node> MCTS::expand(std::shared_ptr<Node> node)
{
   node->setLastExpanded(node->getLastExpanded() + 1);
   auto gameCopy = node->getGame()->clone();

   gameCopy->simulateMove(node->getPossibleMoves()[node->getLastExpanded()]);
   gameCopy->setCurrentPlayer(1 - gameCopy->getCurrentPlayer());

   auto child = std::make_shared<Node>(gameCopy->getPossibleMoves(), gameCopy, node);
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

void MCTS::backpropagate(std::shared_ptr<Node> end, int player, double result)
{
   do
   {
      end->setScore(player, end->getScore(player) + result);
      end->setScore(1-player, end->getScore(1-player) + (1-result));
      end->setSimulations(end->getSimulations() + 1);

      end = end->getParent();
   } while (end != nullptr);
}
