#include "Chess.hpp"
#include "NN_MCTS.hpp"
#include "NetRunner.hpp"
#include "ChessNN.hpp"
#include "NetTrainer.hpp"

int main()
{
   srand(time(NULL));


   std::shared_ptr<Game> real_game = std::make_shared<Chess>();

   std::shared_ptr<Game> game = std::make_shared<Chess>();
   
   auto net = std::static_pointer_cast<Net>(std::make_shared<ChessNet>(32, 3));
   auto device = std::make_shared<torch::Device>(torch::kCPU);
   auto runner = std::make_shared<NetRunner>(net, device, 1);
   auto opt = std::make_shared<torch::optim::Adam>(net->parameters(), 0.001);
   auto interface = std::make_shared<ChessNNInterface>();

   auto trainer = std::make_shared<NetTrainer>(game, interface, net, device, opt, 1, 10);

   trainer->train(2, 1, 1);

   return 0;
}