#include "Chess.hpp"

int main()
{
   srand(time(NULL));
   std::shared_ptr<Game> game = std::make_shared<Chess>();
   game->run(true);

   return 0;
}