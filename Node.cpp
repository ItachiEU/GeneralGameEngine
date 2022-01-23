#include "Node.hpp"

Node::Node(std::vector<std::shared_ptr<Move>> moves, std::shared_ptr<Game> game, std::shared_ptr<Node> ancestor)
{
   std::random_shuffle(moves.begin(), moves.end());
   this->game = game;
   this->possibleMoves = moves;
   this->score = {0.0, 0.0};
   this->parent = ancestor;
   this->simulations = 0;
   this->lastExpanded = -1;
   this->terminal = game->gameStatus(moves) != -1;
}

int Node::getLastExpanded()
{
   return this->lastExpanded;
}

void Node::setLastExpanded(int last)
{
   this->lastExpanded = last;
}

bool Node::getTerminal()
{
   return this->terminal;
}

void Node::setTerminal(bool terminal)
{
   this->terminal = terminal;
}

std::vector<std::shared_ptr<Move>> Node::getPossibleMoves()
{
   return this->possibleMoves;
}

int Node::getSimulations()
{
   return this->simulations;
}

void Node::setSimulations(int simulations)
{
   this->simulations = simulations;
}

double Node::getScore(int player)
{
   return this->score[player];
}

void Node::setScore(int player, double score)
{
   this->score[player] = score;
}

std::unordered_map<int, std::shared_ptr<Node>> &Node::getChildren()
{
   return this->children;
}

std::shared_ptr<Game> Node::getGame()
{
   return this->game;
}

std::shared_ptr<Node> Node::getParent()
{
   return this->parent.lock();
}

const std::vector<double> &Node::getMoveScores()
{
   return this->moveScores;
}

void Node::setMoveScores(const std::vector<double> &moveScores)
{
   this->moveScores = moveScores;
}