#pragma once
#include "Game.hpp"
#include "Node.hpp"

class MCTS
{
private:
   std::shared_ptr<Node> root;
   std::shared_ptr<Node> treePolicy(std::shared_ptr<Node> node);
   std::shared_ptr<Node> bestChild(std::shared_ptr<Node> node, int currentPlayer);
   std::shared_ptr<Node> expand(std::shared_ptr<Node> node);
   double simulate(std::shared_ptr<Node> root);
   void backpropagate(std::shared_ptr<Node> end, int player, double result);

public:
   MCTS(std::shared_ptr<Game> game);
   void run();
   std::pair<std::shared_ptr<Move>, std::shared_ptr<Node>> getBestMove();
   void doMove(std::shared_ptr<Move> move);
   std::shared_ptr<Node> getRoot();
   void setRoot(std::shared_ptr<Node> node);
};