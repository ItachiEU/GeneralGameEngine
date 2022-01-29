#pragma once
#include "MCTS.hpp"
#include "NetRunner.hpp"

struct sample {
    torch::Tensor board;
    torch::Tensor moves;
    torch::Tensor moveScores;
    torch::Tensor result;
};

class NN_Interface
{
public:
   virtual torch::Tensor getNNInput(std::shared_ptr<Game> game, int player) = 0;
   virtual std::vector<double> moveScores(torch::Tensor nn_out, std::vector<std::shared_ptr<Move>> &moves) = 0;
   virtual double boardValue(torch::Tensor nn_out, int player) = 0;
   virtual torch::Tensor movesRepr(std::vector<std::shared_ptr<Move>> &moves) = 0;
   virtual std::vector<sample> augment(sample base) = 0;
};

class NN_MCTS : public MCTS
{
private:
   bool randomness = false;
protected:
   std::shared_ptr<NetRunner> runner;
   std::shared_ptr<NN_Interface> nn_interface;
   double simulate(std::shared_ptr<Node> root);
   std::shared_ptr<Node> treePolicy(std::shared_ptr<Node> node);
   std::shared_ptr<Node> bestChild(std::shared_ptr<Node> node, int currentPlayer);
   std::shared_ptr<Node> expand(std::shared_ptr<Node> node, int move_idx);

public:
   NN_MCTS(std::shared_ptr<Game> game, std::shared_ptr<NetRunner> runner, std::shared_ptr<NN_Interface> interface);
   void setRandomness(bool randomness);
   std::pair<std::shared_ptr<Move>, std::shared_ptr<Node>> getBestMove();
};