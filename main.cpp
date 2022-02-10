#include "Chess.hpp"
#include "NN_MCTS.hpp"
#include "NetRunner.hpp"
#include "ChessNN.hpp"
#include "NetTrainer.hpp"



int main()
{
   srand(time(NULL));

   // std::shared_ptr<Chess> real_game = std::make_shared<Chess>();
   // std::string test = "W1.e4 B1.c6 W2.d4 B2.d5 W3.Nd2 B3.dxe4 W4.Nxe4 B4.Bf5 W5.Ng3 B5.Bg6 W6.h4 B6.h6 W7.Nf3 B7.Nd7 W8.h5 B8.Bh7 W9.Bd3 B9.Bxd3 W10.Qxd3 B10.Ngf6 W11.Bf4 B11.e6 W12.O-O-O B12.Be7 W13.Kb1 B13.O-O W14.Ne4 B14.Nxe4 W15.Qxe4 B15.Nf6 W16.Qe2 B16.Qd5 W17.Ne5 B17.Qe4 W18.Qxe4 B18.Nxe4 W19.Rhe1 B19.Nf6 W20.g4 B20.Rfd8 W21.Be3 B21.Bd6 W22.f3 B22.Rac8 W23.c4 B23.a5 W24.a4 B24.Nd7 W25.Bd2 B25.Bc7 W26.Bc3 B26.Nxe5 W27.dxe5 B27.c5 W28.Kc2 B28.Rxd1 W29.Kxd1 B29.Rd8+ W30.Ke2 B30.Rd7 W31.f4 B31.Bd8 W32.f5 B32.Bg5 W33.f6 B33.b6 W34.Ra1 B34.Rd8 W35.Ra3 B35.gxf6 W36.Rb3 B36.Bf4 W37.Rxb6 B37.Bxe5 W38.Bxe5 B38.fxe5 W39.Rb5 B39.Rd4 W40.Rxc5 B40.Rxg4 W41.b3 B41.Rg3 W42.Rxe5 B42.Rxb3 W43.Rxa5 B43.f5 W44.Ra8+ B44.Kg7 W45.c5 B45.Rc3 W46.Rc8 B46.Ra3 W47.c6 B47.Rxa4 W48.Re8 B48.Rc4 W49.Rxe6 B49.f4 W50.Rg6+ B50.Kh7 W51.Kd3 B51.Rc5 W52.Kd4 B52.Rc1 W53.Ke4 B53.Rc4+ W54.Kd5 B54.Rc3 W55.Kd4 B55.Rc1 W56.Ke4 B56.Rc4+ W57.Kf3 B57.Kh8 W58.Rxh6+ B58.Kg7 W59.Rd6 B59.Kh7 W60.Kg4 B60.Kg7 W61.Rd7+ B61.Kf6 W62.c7 ";
   // std::string move;

   // for (int i = 0; i < test.size(); i++)
   // {
   //    if (isspace(test[i]))
   //    {
   //       auto mv = real_game->getMoveFromStandardNotation(move);
   //       real_game->simulateMove(mv);
   //       std::cout << real_game->printBoard() << std::endl;
   //       real_game->setCurrentPlayer(1 - real_game->getCurrentPlayer());
   //       move = "";
   //    }
   //    else
   //       move.push_back(test[i]);
   // }

   std::shared_ptr<Game> game = std::make_shared<Chess>();

   auto net = std::static_pointer_cast<Net>(std::make_shared<ChessNet>(64, 3));
   net->to(torch::kCUDA);
   auto device = std::make_shared<torch::Device>(torch::kCUDA);
   auto runner = std::make_shared<NetRunner>(net, device, 1);
   auto opt = std::make_shared<torch::optim::Adam>(net->parameters(), 0.0003);
   auto interface = std::make_shared<ChessNNInterface>();

   auto trainer = std::make_shared<NetTrainer>(game, interface, net, device, opt, 1, 64);

   trainer->train(10000, 0, 1, 4);

   return 0;
}