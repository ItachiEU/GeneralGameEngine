#pragma once
#include "Move.hpp"

class ChessMove : public Move
{
private:
   int from_row;
   int from_col;
   int to_row;
   int to_col;
   int piece;
   int color;
   int take_row = -1;
   int take_col = -1;

public:
   ChessMove(int from_row, int from_col, int to_row, int to_col, int color, int piece, int take_row = -1, int take_col = -1);
   int getFromRow();
   int getFromCol();
   int getToRow();
   int getToCol();
   int getPiece();
   int getColor();
   int getTakeRow();
   int getTakeCol();
   void setFromRow(int from_row);
   void setFromCol(int from_col);
   void setToRow(int to_row);
   void setToCol(int to_col);
   void setPiece(int piece);
   void setColor(int color);
   void setTakeRow(int take_row);
   void setTakeCol(int take_col);
};