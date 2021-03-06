#include "ChessMove.hpp"

ChessMove::ChessMove(int from_row, int from_col, int to_row, int to_col, int color, int piece, int take_row, int take_col, bool promotion) : Move()
{
   this->from_row = from_row;
   this->from_col = from_col;
   this->to_col = to_col;
   this->to_row = to_row;
   this->color = color;
   this->piece = piece;
   this->take_row = take_row;
   this->take_col = take_col;
   this->promotion = promotion;
}

int ChessMove::getFromRow()
{
   return this->from_row;
}
int ChessMove::getFromCol()
{
   return this->from_col;
}
int ChessMove::getToRow()
{
   return this->to_row;
}
int ChessMove::getToCol()
{
   return this->to_col;
}
int ChessMove::getPiece()
{
   return this->piece;
}
int ChessMove::getColor()
{
   return this->color;
}
int ChessMove::getTakeRow()
{
   return this->take_row;
}
int ChessMove::getTakeCol()
{
   return this->take_col;
}
bool ChessMove::getPromotion()
{
   return this->promotion;
}
void ChessMove::setFromRow(int from_row)
{
   this->from_row = from_row;
}
void ChessMove::setFromCol(int from_col)
{
   this->from_col = from_col;
}
void ChessMove::setToRow(int to_row)
{
   this->to_row = to_row;
}
void ChessMove::setToCol(int to_col)
{
   this->to_col = to_col;
}
void ChessMove::setPiece(int piece)
{
   this->piece = piece;
}
void ChessMove::setColor(int color)
{
   this->color = color;
}
void ChessMove::setTakeRow(int take_row)
{
   this->take_row = take_row;
}
void ChessMove::setTakeCol(int take_col)
{
   this->take_col = take_col;
}
void ChessMove::setPromotion(bool promotion)
{
   this->promotion = promotion;
}

bool ChessMove::eq(std::shared_ptr<Move> other_m)
{
   std::shared_ptr<ChessMove> other = std::static_pointer_cast<ChessMove>(other_m);
   return (
      this->from_row == other->getFromRow() && 
      this->from_col == other->getFromCol() &&
      this->to_row == other->getToRow() &&
      this->to_col == other->getToCol() &&
      this->color == other->getColor() &&
      this->piece == other->getPiece() &&
      this->take_row == other->getTakeRow() &&
      this->take_col == other->getTakeCol() &&
      this->promotion == other->getPromotion()
   );
}