#pragma once
#include "Game.hpp"
#include "Node.hpp"

class MCTS
{
private:
   std::shared_ptr<Node> root;

public:
   MCTS(Game *game);
   std::shared_ptr<Node> select();
   Node *expand();
   int simulate(std::shared_ptr<Node> root);
   void backpropagate(std::shared_ptr<Node> end);
};