#include "ChessPiece.hpp"

ChessPiece::ChessPiece(int type, int color)
{
   this->type = type;
   this->color = color;
}
ChessPiece::ChessPiece()
{
   this->type = Piece::EMPTY;
   this->color = -1;
}
int ChessPiece::getType()
{
   return this->type;
}
int ChessPiece::getColor()
{
   return this->color;
}
void ChessPiece::setType(int type)
{
   this->type = type;
}
void ChessPiece::setColor(int color)
{
   this->color = color;
}