#pragma once
#include <bits/stdc++.h>
#include "Game.hpp"
#include "Move.hpp"

class Node
{
private:
   std::vector<double> score;
   int simulations;
   int lastExpanded;
   bool terminal;
   std::shared_ptr<Game> game;
   std::shared_ptr<Node> parent;
   std::vector<std::shared_ptr<Move>> possibleMoves;
   std::unordered_map<int, std::shared_ptr<Node>> children;

public:
   Node(std::vector<std::shared_ptr<Move>> moves, std::shared_ptr<Game> game, std::shared_ptr<Node> ancestor = nullptr);
   int getLastExpanded();
   void setLastExpanded(int last);
   bool getTerminal();
   void setTerminal(bool terminal);
   std::vector<std::shared_ptr<Move>> getPossibleMoves();
   int getSimulations();
   void setSimulations(int simulations);
   double getScore(int player);
   void setScore(int player, double score);
   std::unordered_map<int, std::shared_ptr<Node>> &getChildren();
   std::shared_ptr<Game> getGame();
   std::shared_ptr<Node> getParent();
};