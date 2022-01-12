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
   void setCurrentPlayer(int player);
   virtual void run(bool debug = false) = 0;
   virtual void test() = 0;
   virtual std::vector<std::shared_ptr<Move>> getPossibleMoves() = 0;
   virtual void simulateMove(std::shared_ptr<Move> move) = 0;
   virtual int gameStatus(std::vector<std::shared_ptr<Move>> moves) = 0;
   virtual std::shared_ptr<Game> clone() = 0;
};