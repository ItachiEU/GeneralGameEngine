#include "Move.hpp"

Move::Move(int from_row, int from_col, int to_row, int to_col, int color, int piece, int take_row, int take_col)
{
   this->from_row = from_row;
   this->from_col = from_col;
   this->to_col = to_col;
   this->to_row = to_row;
   this->color = color;
   this->piece = piece;
   this->take_row = take_row;
   this->take_col = take_col;
}

int Move::getFromRow()
{
   return this->from_row;
}
int Move::getFromCol()
{
   return this->from_col;
}
int Move::getToRow()
{
   return this->to_row;
}
int Move::getToCol()
{
   return this->to_col;
}
int Move::getPiece()
{
   return this->piece;
}
int Move::getColor()
{
   return this->color;
}
int Move::getTakeRow()
{
   return this->take_row;
}
int Move::getTakeCol()
{
   return this->take_col;
}
void Move::setFromRow(int from_row)
{
   this->from_row = from_row;
}
void Move::setFromCol(int from_col)
{
   this->from_col = from_col;
}
void Move::setToRow(int to_row)
{
   this->to_row = to_row;
}
void Move::setToCol(int to_col)
{
   this->to_col = to_col;
}
void Move::setPiece(int piece)
{
   this->piece = piece;
}
void Move::setColor(int color)
{
   this->color = color;
}
void Move::setTakeRow(int take_row)
{
   this->take_row = take_row;
}
void Move::setTakeCol(int take_col)
{
   this->take_col = take_col;
}