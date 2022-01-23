#pragma once
#include "MCTS.hpp"
#include "NetRunner.hpp"

class NN_Interface
{
public:
   virtual torch::Tensor getNNInput(std::shared_ptr<Game> game, int player) = 0;
   virtual std::vector<double> moveScores(torch::Tensor nn_out, std::vector<std::shared_ptr<Move>> &moves) = 0;
   virtual double boardValue(torch::Tensor nn_out, int player) = 0;
   virtual torch::Tensor movesRepr(std::vector<std::shared_ptr<Move>> &moves) = 0;
};

class NN_MCTS : public MCTS
{
protected:
   std::shared_ptr<NetRunner> runner;
   std::shared_ptr<NN_Interface> nn_interface;
   double simulate(std::shared_ptr<Node> root);
   std::shared_ptr<Node> treePolicy(std::shared_ptr<Node> node);
   std::shared_ptr<Node> bestChild(std::shared_ptr<Node> node, int currentPlayer);
   std::shared_ptr<Node> expand(std::shared_ptr<Node> node, int move_idx);

public:
   NN_MCTS(std::shared_ptr<Game> game, std::shared_ptr<NetRunner> runner, std::shared_ptr<NN_Interface> interface);
};