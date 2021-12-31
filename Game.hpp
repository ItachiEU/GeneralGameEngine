#pragma once
#include "Move.hpp"
#include <bits/stdc++.h>

class Game
{
private:
protected:
   int currentPlayer;

public:
   int getCurrentPlayer();
   virtual void run(bool debug = false) = 0;
   virtual std::vector<Move *> getPossibleMoves() = 0;
   virtual void simulateMove(Move *move) = 0;
   virtual int gameStatus(std::vector<Move *> moves) = 0;
};