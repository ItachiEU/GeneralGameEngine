#include "ChessNN2.hpp"
#include "Chess.hpp"

ChessNet2::ChessNet2(int channels, int kernel_size) : Net()
{
    // each position has 6*2 possible pieces
    auto in_options = torch::nn::Conv2dOptions(12, channels, 1)
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
    res8 = register_module("res8", std::make_shared<ResidualLayer>(channels, kernel_size));
    res9 = register_module("res9", std::make_shared<ResidualLayer>(channels, kernel_size));
    res10 = register_module("res10", std::make_shared<ResidualLayer>(channels, kernel_size));
    out_conv_from = register_module("out_conv_from", torch::nn::Conv2d(out_options));
    out_conv_to = register_module("out_conv_to", torch::nn::Conv2d(out_options));
    out_lin_1 = register_module("out_lin_1", torch::nn::Linear(channels * 8 * 8, 256));
    out_lin_2 = register_module("out_lin_2", torch::nn::Linear(256, 6)); // 4 promote options + 1 for value
}

torch::Tensor ChessNet2::forward(torch::Tensor x)
{
    torch::Tensor y = in_conv->forward(x);
    y = res1->forward(y);
    y = res2->forward(y);
    y = res3->forward(y);
    y = res4->forward(y);
    y = res5->forward(y);
    y = res6->forward(y);
    y = res7->forward(y);
    y = res8->forward(y);
    y = res9->forward(y);
    y = res10->forward(y);
    auto pre_values = out_conv_from->forward(y).reshape({-1, 64, 64}).transpose(1, 2);
    // std::cout << targets.sizes() << std::endl;
    auto board_values = pre_values.reshape({-1, 64 * 64});

    auto ylin = out_lin_1->forward(y.reshape({-1, 8 * 8 * y.size(1)}));
    auto scores = out_lin_2->forward(ylin);
    return torch::cat({board_values, scores}, 1);
}

torch::Tensor ChessNet2::loss(torch::Tensor input, torch::Tensor moves, torch::Tensor move_masks, torch::Tensor move_scores, torch::Tensor result)
{

    auto nn_out = this->forward(input);

    auto board_scores = nn_out.slice(1, 0, 8 * 8 * 8 * 8);
    // auto promote_scores = nn_out.slice(1, 8*8*8*8, 8*8*8*8 + 4);
    auto values = nn_out.slice(1, 8 * 8 * 8 * 8 + 4, 8 * 8 * 8 * 8 + 5);

    auto gathered_scores = torch::gather(board_scores, 1, moves) + move_masks;
    auto probs = torch::log(torch::softmax(gathered_scores, 1) + 1e-7);

    // std :: cout << probs.sizes() << std::endl;
    // std :: cout << move_scores << std::endl;

    auto move_loss = probs.unsqueeze(-2).matmul(move_scores.unsqueeze(-1));
    // std :: cout << move_loss.sizes() << std::endl;
    // std :: cout << move_loss << std::endl;
    auto value_loss = torch::binary_cross_entropy(torch::sigmoid(values), result);
    return (-move_loss.mean() + value_loss).sum();
}

std::map<int, std::pair<int, int>> forward = {
    {0, {0, 1}},
    {1, {1, 1}},
    {2, {1, 0}},
    {3, {1, -1}},
    {4, {0, -1}},
    {5, {-1, -1}},
    {6, {-1, 0}},
    {7, {-1, 1}},
    {8, {0, 2}},
    {9, {2, 2}},
    {10, {2, 0}},
    {11, {2, -2}},
    {12, {0, -2}},
    {13, {-2, -2}},
    {14, {-2, 0}},
    {15, {-2, 2}},
    {16, {0, 3}},
    {17, {3, 3}},
    {18, {3, 0}},
    {19, {3, -3}},
    {20, {0, -3}},
    {21, {-3, -3}},
    {22, {-3, 0}},
    {23, {-3, 3}},
    {24, {0, 4}},
    {25, {4, 4}},
    {26, {4, 0}},
    {27, {4, -4}},
    {28, {0, -4}},
    {29, {-4, -4}},
    {30, {-4, 0}},
    {31, {-4, 4}},
    {32, {0, 5}},
    {33, {5, 5}},
    {34, {5, 0}},
    {35, {5, -5}},
    {36, {0, -5}},
    {37, {-5, -5}},
    {38, {-5, 0}},
    {39, {-5, 5}},
    {40, {0, 6}},
    {41, {6, 6}},
    {42, {6, 0}},
    {43, {6, -6}},
    {44, {0, -6}},
    {45, {-6, -6}},
    {46, {-6, 0}},
    {47, {-6, 6}},
    {48, {0, 7}},
    {49, {7, 7}},
    {50, {7, 0}},
    {51, {7, -7}},
    {52, {0, -7}},
    {53, {-7, -7}},
    {54, {-7, 0}},
    {55, {-7, 7}},
    {56, {1, 2}},
    {57, {2, 1}},
    {58, {-1, 2}},
    {59, {-2, 1}},
    {60, {1, -2}},
    {61, {2, -1}},
    {62, {-1, -2}},
    {63, {-2, -1}},
};

std::map<std::pair<int, int>, int> backward = {
    {{0, 1}, 0},
    {{1, 1}, 1},
    {{1, 0}, 2},
    {{1, -1}, 3},
    {{0, -1}, 4},
    {{-1, -1}, 5},
    {{-1, 0}, 6},
    {{-1, 1}, 7},
    {{0, 2}, 8},
    {{2, 2}, 9},
    {{2, 0}, 10},
    {{2, -2}, 11},
    {{0, -2}, 12},
    {{-2, -2}, 13},
    {{-2, 0}, 14},
    {{-2, 2}, 15},
    {{0, 3}, 16},
    {{3, 3}, 17},
    {{3, 0}, 18},
    {{3, -3}, 19},
    {{0, -3}, 20},
    {{-3, -3}, 21},
    {{-3, 0}, 22},
    {{-3, 3}, 23},
    {{0, 4}, 24},
    {{4, 4}, 25},
    {{4, 0}, 26},
    {{4, -4}, 27},
    {{0, -4}, 28},
    {{-4, -4}, 29},
    {{-4, 0}, 30},
    {{-4, 4}, 31},
    {{0, 5}, 32},
    {{5, 5}, 33},
    {{5, 0}, 34},
    {{5, -5}, 35},
    {{0, -5}, 36},
    {{-5, -5}, 37},
    {{-5, 0}, 38},
    {{-5, 5}, 39},
    {{0, 6}, 40},
    {{6, 6}, 41},
    {{6, 0}, 42},
    {{6, -6}, 43},
    {{0, -6}, 44},
    {{-6, -6}, 45},
    {{-6, 0}, 46},
    {{-6, 6}, 47},
    {{0, 7}, 48},
    {{7, 7}, 49},
    {{7, 0}, 50},
    {{7, -7}, 51},
    {{0, -7}, 52},
    {{-7, -7}, 53},
    {{-7, 0}, 54},
    {{-7, 7}, 55},
    {{1, 2}, 56},
    {{2, 1}, 57},
    {{-1, 2}, 58},
    {{-2, 1}, 59},
    {{1, -2}, 60},
    {{2, -1}, 61},
    {{-1, -2}, 62},
    {{-2, -1}, 63},
};

torch::Tensor ChessNNInterface2::getNNInput(std::shared_ptr<Game> game, int player)
{
    std::shared_ptr<Chess> chess_game = std::static_pointer_cast<Chess>(game);
    auto board = chess_game->getBoard();
    auto board_tensor = torch::zeros({1, 12, 8, 8});
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            auto piece = board[i][j];
            if (piece.getType())
            {
                board_tensor[0][piece.getType() - 1 + 6 * piece.getColor()][i][j] = 1;
            }
        }
    }
    // std::cout << board_tensor << std::endl;
    return board_tensor;
}

std::vector<double> ChessNNInterface2::moveScores(torch::Tensor nn_out, std::vector<std::shared_ptr<Move>> &moves)
{
    auto board_scores = nn_out.slice(1, 0, 8 * 8 * 8 * 8).reshape({8, 8, 8*8});
    auto promote_scores = nn_out.slice(1, 8 * 8 * 8 * 8, 8 * 8 * 8 * 8 + 4); // to be added once flag is avaliable

    int s = moves.size();
    auto scores = torch::zeros({s});

    for (int i = 0; i < s; i++)
    {
        auto move = std::static_pointer_cast<ChessMove>(moves[i]);
        int dx = move->getToRow() - move->getFromRow();
        int dy = move->getToCol() - move->getFromCol();
        int loc = backward[{dx, dy}];
        scores[i] = board_scores[move->getFromRow()][move->getFromCol()][loc];
        // std::cout << scores[i].item().toDouble() << " " << move->getFromRow() << move->getFromCol() << move->getToRow() << move->getToCol() << std::endl;
    }

    scores = torch::softmax(scores, 0);

    // std::cout << "scores: " << scores << std::endl;

    std::vector<double> scores_vec;

    for (int i = 0; i < s; i++)
    {
        scores_vec.push_back(scores[i].item().toDouble());
    }

    return scores_vec;
}

double ChessNNInterface2::boardValue(torch::Tensor nn_out, int player)
{
    // network always predicts the value of the white player
    double value = torch::sigmoid(nn_out[0][8 * 8 * 8 * 8 + 4]).item().toDouble();
    if (player == 1)
    {
        value = 1 - value;
    }
    return value;
}

torch::Tensor ChessNNInterface2::movesRepr(std::vector<std::shared_ptr<Move>> &moves)
{
    torch::Tensor moves_tensor = torch::zeros({1, (int)moves.size()}, torch::kInt64);
    int s = moves.size();
    for (int i = 0; i < s; i++)
    {
        auto move = std::static_pointer_cast<ChessMove>(moves[i]);
        int dx = move->getToRow() - move->getFromRow();
        int dy = move->getToCol() - move->getFromCol();
        int loc = backward[{dx, dy}];
        moves_tensor[0][i] = move->getFromRow() * 512 + move->getFromCol() * 64 + loc;
    }
    return moves_tensor;
}

static torch::Tensor decodeMoves(torch::Tensor moves)
{
    int s = moves.size(1);
    auto decoded_moves = torch::zeros({1, s, 4}, torch::kInt64);

    int divider = 512;

    using namespace torch::indexing;

    for (int i = 0; i < 4; i++)
    {
        decoded_moves.index_put_({0, Slice(), i}, (moves[0] / divider));
        moves = moves % divider;
        divider /= 8;
    }

    return decoded_moves;
}

static torch::Tensor encodeMoves(torch::Tensor moves)
{
    int s = moves.size(1);
    auto encoded_moves = torch::zeros({1, s}, torch::kInt64);

    int divider = 512;

    using namespace torch::indexing;

    for (int i = 0; i < 4; i++)
    {
        encoded_moves = encoded_moves + moves.index({Slice(), Slice(), i}) * divider;
        divider /= 8;
    }

    return encoded_moves;
}

std::vector<sample> ChessNNInterface2::augment(sample base)
{
    std::vector<sample> samples;
    samples.push_back(base);

    return samples;
}