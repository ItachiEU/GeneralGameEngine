enum Piece
{
   EMPTY = 0,
   PAWN = 1,
   KNIGHT = 2,
   BISHOP = 3,
   ROOK = 4,
   QUEEN = 5,
   KING = 6
};

enum Player
{
   WHITE = 0,
   BLACK = 1
};

class ChessPiece
{
private:
   int type;
   int color;

public:
   ChessPiece(int type, int color);
   ChessPiece();
   int getType();
   int getColor();
   void setType(int type);
   void setColor(int color);
};