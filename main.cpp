#include "Chess.hpp"
#include "NN_MCTS.hpp"
#include "NetRunner.hpp"
#include "ChessNN2.hpp"
#include "NetTrainer.hpp"
#include "UCI.hpp"

int main(int argc, char* argv[])
{
   srand(time(NULL));

   std::string arg = "";
   if(argc > 1)
   {
      arg = argv[1];
   }

   if (argc == 1)
   {
      uci_loop();
   }
   else if (arg == "train")
   {
      std::shared_ptr<Game> game = std::make_shared<Chess>();

      auto net = std::static_pointer_cast<Net>(std::make_shared<ChessNet2>(128, 3));
      net->to(torch::kCUDA);
      auto device = std::make_shared<torch::Device>(torch::kCUDA);
      auto runner = std::make_shared<NetRunner>(net, device, 1);
      auto opt = std::make_shared<torch::optim::Adam>(net->parameters(), 0.0003);
      auto interface = std::make_shared<ChessNNInterface2>();

      auto trainer = std::make_shared<NetTrainer>(game, interface, net, device, opt, 1, 128);

      trainer->train(40000, 0, 1, 4);
   }
   else 
   {
      std::cout << "Invalid argument" << std::endl;
      return 1;
   }

   return 0;
}