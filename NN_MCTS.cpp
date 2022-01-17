#include "NN_MCTS.hpp"

NN_MCTS::NN_MCTS(std::shared_ptr<Game> game, std::shared_ptr<NetRunner> runner, std::shared_ptr<NN_Interface> interface) : MCTS(game)
{
   this->runner = runner;
   this->nn_interface = interface;

   auto nn_input = this->nn_interface->getNNInput(game, game->getCurrentPlayer());
   auto nn_out = this->runner->request_run(nn_input);
   auto moves = game->getPossibleMoves();
   auto move_scores = this->nn_interface->moveScores(nn_out, moves);

   getRoot()->setMoveScores(move_scores);
}

double NN_MCTS::simulate(std::shared_ptr<Node> root)
{
   auto game = root->getGame();
   auto current_player = game->getCurrentPlayer();

   if (root->getTerminal())
   {
      return root->getScore(current_player);
   }

   auto nn_input = this->nn_interface->getNNInput(game, current_player);
   auto nn_out = this->runner->request_run(nn_input);
   auto moves = game->getPossibleMoves();
   auto move_scores = this->nn_interface->moveScores(nn_out, moves);

   root->setMoveScores(move_scores);

   double score = this->nn_interface->boardValue(nn_out, current_player);

   root->setScore(current_player, score);
   root->setScore(1 - current_player, 1 - score);
   return score;
}

std::shared_ptr<Node> NN_MCTS::treePolicy(std::shared_ptr<Node> node)
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
std::shared_ptr<Node> bestChild(std::shared_ptr<Node> node, int currentPlayer);
std::shared_ptr<Node> expand(std::shared_ptr<Node> node);