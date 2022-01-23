#include "NN_MCTS.hpp"

NN_MCTS::NN_MCTS(std::shared_ptr<Game> game, std::shared_ptr<NetRunner> runner, std::shared_ptr<NN_Interface> interface) : MCTS(game)
{
   this->runner = runner;
   this->nn_interface = interface;
   auto g = this->root->getGame();

   auto nn_input = this->nn_interface->getNNInput(g, g->getCurrentPlayer());
   auto nn_out = this->runner->request_run(nn_input);
   auto moves = this->root->getPossibleMoves();
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

   // std::cout << "score = " << score << std::endl;

   root->setScore(current_player, score);
   root->setScore(1 - current_player, 1 - score);
   return score;
}

std::shared_ptr<Node> NN_MCTS::treePolicy(std::shared_ptr<Node> node)
{
   int currentPlayer = node->getGame()->getCurrentPlayer();
   while (node->getTerminal() == false)
   {
      if (node->getSimulations() == 0) break;
      node = this->bestChild(node, currentPlayer);
      currentPlayer = 1 - currentPlayer;
   }
   return node;
}

// Since expansions are also guided by the neural network, bestChild is now responsible for expanding
std::shared_ptr<Node> NN_MCTS::bestChild(std::shared_ptr<Node> node, int currentPlayer){
   double bestScore = -1.0;
   int chosenIndex = -1;
   bool best_expand = false;

   int s = node->getPossibleMoves().size();
   assert((int)node->getMoveScores().size() == s);
   double N_P = node->getSimulations();
   for (int i = 0; i<s; i++)
   {
      double W = 0;
      double N = 0;
      bool expand = true;
      if(node->getChildren().find(i) != node->getChildren().end())
      {
         W = node->getChildren()[i]->getScore(currentPlayer);
         N = node->getChildren()[i]->getSimulations();
         expand = false;
      }
      double c = 0.1;
      double score = (W/(N + 0.00001)) + c*node->getMoveScores()[i]*sqrt(N_P)/(1+N);

      // std::cerr << score << std::endl; 

      if(score > bestScore)
      {
         bestScore = score;
         chosenIndex = i;
         best_expand = expand;
      }
   }
   
   if(best_expand)
   {
      // std::cerr << "expanding" << std::endl;
      return this->expand(node, chosenIndex);
   }
   else
   {
      // std::cerr << "not expanding? " << bestScore << std::endl;
      return node->getChildren()[chosenIndex];
   }
}

std::pair<std::shared_ptr<Move>, std::shared_ptr<Node>> NN_MCTS::getBestMove(){
   if(!this->randomness){
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
   } else {
      int acc = 0;
      int random_pos = rand() % (root->getSimulations() - 1);
      for (auto son : root->getChildren())
      {
         acc += son.second->getSimulations();
         if(acc >= random_pos)
         {
            return make_pair(root->getPossibleMoves()[son.first], son.second);
         }
      }
      assert (false && "No child found");
   }
}

std::shared_ptr<Node> NN_MCTS::expand(std::shared_ptr<Node> node, int move_index)
{
   auto gameCopy = node->getGame()->clone();

   gameCopy->simulateMove(node->getPossibleMoves()[move_index]);
   gameCopy->setCurrentPlayer(1 - gameCopy->getCurrentPlayer());

   auto child = std::make_shared<Node>(gameCopy->getPossibleMoves(), gameCopy, node);
   node->getChildren()[move_index] = child;
   if (child->getGame()->gameStatus(gameCopy->getPossibleMoves()) != -1)
      child->setTerminal(true);

   return child;
}

void NN_MCTS::setRandomness(bool randomness)
{
   this->randomness = randomness;
}

