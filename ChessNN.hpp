#pragma once
#include "NN_MCTS.hpp"

class ResidualLayer : public torch::nn::Module
{
private:
   torch::nn::Conv2d conv1 = nullptr;
   torch::nn::Conv2d conv2 = nullptr;
   torch::nn::LayerNorm norm = nullptr;

public:
   ResidualLayer(int in_channels, int kernel_size);
   torch::Tensor forward(torch::Tensor x);
};

class ChessNet : public Net
{
private:
   torch::nn::Conv2d in_conv = nullptr;
   std::shared_ptr<ResidualLayer> res1;
   std::shared_ptr<ResidualLayer> res2;
   std::shared_ptr<ResidualLayer> res3;
   std::shared_ptr<ResidualLayer> res4;
   std::shared_ptr<ResidualLayer> res5;
   std::shared_ptr<ResidualLayer> res6;
   std::shared_ptr<ResidualLayer> res7;
   torch::nn::Conv2d out_conv = nullptr;
   torch::nn::Linear out_linear = nullptr;

public:
   ChessNet(int channels, int kernel_size);
   torch::Tensor forward(torch::Tensor x);
   torch::Tensor loss(torch::Tensor input, torch::Tensor moves, torch::Tensor masks, torch::Tensor move_scores, torch::Tensor result);
};

class ChessNNInterface : public NN_Interface
{
public:
   torch::Tensor getNNInput(std::shared_ptr<Game> game, int player);
   std::vector<double> moveScores(torch::Tensor nn_out, std::vector<std::shared_ptr<Move>> &moves);
   double boardValue(torch::Tensor nn_out, int player);
   torch::Tensor movesRepr(std::vector<std::shared_ptr<Move>> &moves);
};