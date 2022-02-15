#pragma once
#include "NN_MCTS.hpp"
#include "ChessNN.hpp"

class ChessNet2 : public Net
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
   std::shared_ptr<ResidualLayer> res8;
   std::shared_ptr<ResidualLayer> res9;
   std::shared_ptr<ResidualLayer> res10;
   torch::nn::Conv2d out_conv_from = nullptr;
   torch::nn::Conv2d out_conv_to = nullptr;
   torch::nn::Linear out_lin_1 = nullptr;
   torch::nn::Linear out_lin_2 = nullptr;

public:
   ChessNet2(int channels, int kernel_size);
   torch::Tensor forward(torch::Tensor x);
   torch::Tensor loss(torch::Tensor input, torch::Tensor moves, torch::Tensor masks, torch::Tensor move_scores, torch::Tensor result);
};

class ChessNNInterface2 : public NN_Interface
{
public:
   torch::Tensor getNNInput(std::shared_ptr<Game> game, int player);
   std::vector<double> moveScores(torch::Tensor nn_out, std::vector<std::shared_ptr<Move>> &moves);
   double boardValue(torch::Tensor nn_out, int player);
   torch::Tensor movesRepr(std::vector<std::shared_ptr<Move>> &moves);
   std::vector<sample> augment(sample base);
};