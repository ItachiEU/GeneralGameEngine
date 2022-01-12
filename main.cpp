#include "Chess.hpp"

int main()
{
   srand(time(NULL));
   std::shared_ptr<Game> game = std::make_shared<Chess>();
   // game->run(false);
   game->test();

   return 0;
}