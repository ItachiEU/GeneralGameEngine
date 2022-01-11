#include "Game.hpp"

int Game::getCurrentPlayer()
{
   return this->currentPlayer;
}

void Game::setCurrentPlayer(int player)
{
   this->currentPlayer = player;
}