#include "ChessNN.hpp"
#include "Chess.hpp"

ResidualLayer::ResidualLayer(int in_channels, int kernel_size) : torch::nn::Module()
{
    auto options = torch::nn::Conv2dOptions(in_channels, in_channels, kernel_size)
        .stride(1)
        .padding(1)
        .bias(true);

    auto norm_options = torch::nn::LayerNormOptions({in_channels, 8, 8});

    conv1 = register_module("conv1", torch::nn::Conv2d(options));
    conv2 = register_module("conv2", torch::nn::Conv2d(options));
    norm = register_module("norm", torch::nn::LayerNorm(norm_options));
}

torch::Tensor ResidualLayer::forward(torch::Tensor x)
{
    torch::Tensor y = torch::relu(conv1->forward(x));
    y = conv2->forward(y);
    y = norm->forward(y + x);
    return y;
}

ChessNet::ChessNet(int channels, int kernel_size) : Net()
{   
    // each position has 7 piece options (one-hot) + 1 for piece color
    auto in_options = torch::nn::Conv2dOptions(8, channels, 1) 
        .stride(1)
        .padding(0)
        .bias(true);

    auto out_options = torch::nn::Conv2dOptions(channels, 64, 1)
        .stride(1)
        .padding(0)
        .bias(true);

    in_conv = register_module("in_conv", torch::nn::Conv2d(in_options));
    res1 = register_module("res1", std::make_shared<ResidualLayer>(channels, kernel_size));
    res2 = register_module("res2", std::make_shared<ResidualLayer>(channels, kernel_size));
    res3 = register_module("res3", std::make_shared<ResidualLayer>(channels, kernel_size));
    res4 = register_module("res4", std::make_shared<ResidualLayer>(channels, kernel_size));
    res5 = register_module("res5", std::make_shared<ResidualLayer>(channels, kernel_size));
    res6 = register_module("res6", std::make_shared<ResidualLayer>(channels, kernel_size));
    res7 = register_module("res7", std::make_shared<ResidualLayer>(channels, kernel_size));
    out_conv = register_module("out_conv", torch::nn::Conv2d(out_options)); // 64 x 8 x 8 ~ start_pos x end_pos
    out_linear = register_module("out_linear", torch::nn::Linear(channels, 6)); // 4 promote options + 1 for value
}

torch::Tensor ChessNet::forward(torch::Tensor x)
{
    torch::Tensor y = in_conv->forward(x);
    y = res1->forward(y);
    y = res2->forward(y);
    y = res3->forward(y);
    y = res4->forward(y);
    y = res5->forward(y);
    y = res6->forward(y);
    y = res7->forward(y);
    auto move_values = out_conv->forward(y).permute({0,2,3,1}).flatten(1,-1);
    auto scores = out_linear->forward(y.mean({2,3}));
    return torch::cat({move_values, scores}, 1);
}

torch::Tensor ChessNet::loss(torch::Tensor input, torch::Tensor moves, torch::Tensor move_masks, torch::Tensor move_scores, torch::Tensor result){

    auto nn_out = this->forward(input);

    auto board_scores = nn_out.slice(1, 0, 8*8*8*8);
    // auto promote_scores = nn_out.slice(1, 8*8*8*8, 8*8*8*8 + 4);
    auto values = nn_out.slice(1, 8*8*8*8 + 4, 8*8*8*8 + 5);

    auto gathered_scores = torch::gather(board_scores, 1, moves) + move_masks;
    auto probs = torch::log(torch::softmax(gathered_scores, 1)+1e-7);

    auto move_loss = probs.unsqueeze(-2).matmul(move_scores.unsqueeze(-1));
    auto value_loss = torch::mse_loss(torch::sigmoid(values), result);
    return (-move_loss.mean() + value_loss).sum();
}

torch::Tensor ChessNNInterface::getNNInput(std::shared_ptr<Game> game, int player)
{
    std::shared_ptr<Chess> chess_game = std::static_pointer_cast<Chess>(game);
    auto board = chess_game->getBoard();
    auto board_tensor = torch::zeros({1, 8, 8, 8});
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            auto piece = board[i][j];
            board_tensor[0][piece.getType()][i][j] = 1;
            board_tensor[0][7][i][j] = piece.getColor();
        }
    }
    return board_tensor;
}

std::vector<double> ChessNNInterface::moveScores(torch::Tensor nn_out, std::vector<std::shared_ptr<Move>> &moves)
{
    auto board_scores = nn_out.slice(1, 0, 8*8*8*8).reshape({8, 8, 8, 8});
    auto promote_scores = nn_out.slice(1, 8*8*8*8, 8*8*8*8 + 4); // to be added once flag is avaliable

    int s = moves.size();
    auto scores = torch::zeros({s});

    for(int i = 0; i < s; i++)
    {
        auto move = std::static_pointer_cast<ChessMove>(moves[i]);
        scores[i] = board_scores[move->getFromRow()][move->getFromCol()][move->getToRow()][move->getToCol()];
        // std::cout << scores[i].item().toDouble() << " " << move->getFromRow() << move->getFromCol() << move->getToRow() << move->getToCol() << std::endl;
    }

    scores = torch::softmax(scores, 0);

    std::vector<double> scores_vec;

    for(int i = 0; i < s; i++)
    {
        scores_vec.push_back(scores[i].item().toDouble());
    }

    return scores_vec;
}

double ChessNNInterface::boardValue(torch::Tensor nn_out, int player)
{
    // network always predicts the value of the white player
    double value = torch::sigmoid(nn_out[0][8*8*8*8 + 4]).item().toDouble();
    if (player == 1)
    {
        value = 1 - value;
    }
    return value;
}

torch::Tensor ChessNNInterface::movesRepr(std::vector<std::shared_ptr<Move>> &moves)
{
    torch::Tensor moves_tensor = torch::zeros({1,(int)moves.size()}, torch::kInt64);
    int s = moves.size();
    for (int i = 0; i < s; i++)
    {
        auto move = std::static_pointer_cast<ChessMove>(moves[i]);
        moves_tensor[0][i] = move->getFromRow() * 256 + move->getFromCol()*64 + move->getToRow()*8 + move->getToCol();
    }
    return moves_tensor;
}