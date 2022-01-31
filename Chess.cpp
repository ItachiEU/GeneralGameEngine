#include "Chess.hpp"
#include "MCTS.hpp"

// helper functions

void fillPawns(std::vector<std::vector<ChessPiece>> &board)
{
   // black and white pawns
   for (int col = 0; col <= 7; col++)
   {
      board[6][col].setColor(BLACK);
      board[6][col].setType(PAWN);
      board[1][col].setColor(WHITE);
      board[1][col].setType(PAWN);
   }
}

void fillKnights(std::vector<std::vector<ChessPiece>> &board)
{
   board[0][1].setColor(WHITE);
   board[0][1].setType(KNIGHT);
   board[0][6].setColor(WHITE);
   board[0][6].setType(KNIGHT);
   board[7][1].setColor(BLACK);
   board[7][1].setType(KNIGHT);
   board[7][6].setColor(BLACK);
   board[7][6].setType(KNIGHT);
}

void fillBishops(std::vector<std::vector<ChessPiece>> &board)
{
   board[0][2].setColor(WHITE);
   board[0][2].setType(BISHOP);
   board[0][5].setColor(WHITE);
   board[0][5].setType(BISHOP);
   board[7][2].setColor(BLACK);
   board[7][2].setType(BISHOP);
   board[7][5].setColor(BLACK);
   board[7][5].setType(BISHOP);
}

void fillRooks(std::vector<std::vector<ChessPiece>> &board)
{
   board[0][0].setColor(WHITE);
   board[0][0].setType(ROOK);
   board[0][7].setColor(WHITE);
   board[0][7].setType(ROOK);
   board[7][0].setColor(BLACK);
   board[7][0].setType(ROOK);
   board[7][7].setColor(BLACK);
   board[7][7].setType(ROOK);
}

void fillRoyal(std::vector<std::vector<ChessPiece>> &board)
{
   board[0][4].setColor(WHITE);
   board[0][4].setType(QUEEN);
   board[0][3].setColor(WHITE);
   board[0][3].setType(KING);
   board[7][4].setColor(BLACK);
   board[7][4].setType(QUEEN);
   board[7][3].setColor(BLACK);
   board[7][3].setType(KING);
}

// private
void Chess::initBoard(std::vector<std::vector<ChessPiece>> &board)
{
   // this->board.assign(8, std::vector<ChessPiece>(8));
   fillPawns(this->board);
   fillKnights(this->board);
   fillBishops(this->board);
   fillRooks(this->board);
   fillRoyal(this->board);
}

std::vector<std::shared_ptr<ChessMove>> Chess::getPawnMoves(int row, int column)
{
   // check for en passant
   std::vector<std::shared_ptr<ChessMove>> moves;
   if (this->movesHistory.size() > 0)
   {
      auto lastMove = this->movesHistory.back();
      if (lastMove->getPiece() == PAWN and lastMove->getToRow() == row and abs(column - lastMove->getToCol()) == 1 and abs(lastMove->getToRow() - lastMove->getFromRow()) == 2)
      {
         int to_row = this->currentPlayer == WHITE ? row + 1 : row - 1;
         moves.push_back(std::make_shared<ChessMove>(row, column, to_row, lastMove->getToCol(), currentPlayer, PAWN, lastMove->getToRow(), lastMove->getToCol()));
      }
   }
   if (currentPlayer == WHITE)
   {
      if (row + 1 < 8 and this->board[row + 1][column].getType() == EMPTY) // move one up
      {
         if (row + 1 == 7) // promote
         {
            for (int piece = KNIGHT; piece <= QUEEN; piece++) // consider different promotion options
               moves.push_back(std::make_shared<ChessMove>(row, column, row + 1, column, currentPlayer, piece, -1, -1));
         }
         else
            moves.push_back(std::make_shared<ChessMove>(row, column, row + 1, column, currentPlayer, PAWN, -1, -1));
      }
      if (row == 1 and this->board[row + 1][column].getType() == EMPTY and this->board[row + 2][column].getType() == EMPTY) // move 2 up
      {
         moves.push_back(std::make_shared<ChessMove>(row, column, row + 2, column, currentPlayer, PAWN, -1, -1));
      }
      if (row + 1 < 8 and column - 1 >= 0 and this->board[row + 1][column - 1].getColor() == BLACK) // take top right
      {
         if (row + 1 == 7) // promote
         {
            for (int piece = KNIGHT; piece <= QUEEN; piece++) // consider different promotion options
               moves.push_back(std::make_shared<ChessMove>(row, column, row + 1, column - 1, currentPlayer, piece, row + 1, column - 1));
         }
         else
            moves.push_back(std::make_shared<ChessMove>(row, column, row + 1, column - 1, currentPlayer, PAWN, row + 1, column - 1));
      }
      if (row + 1 < 8 and column + 1 < 8 and this->board[row + 1][column + 1].getColor() == BLACK) // take top left
      {
         if (row + 1 == 7) // promote
         {
            for (int piece = KNIGHT; piece <= QUEEN; piece++) // consider different promotion options
               moves.push_back(std::make_shared<ChessMove>(row, column, row + 1, column + 1, currentPlayer, piece, row + 1, column + 1));
         }
         else
            moves.push_back(std::make_shared<ChessMove>(row, column, row + 1, column + 1, currentPlayer, PAWN, row + 1, column + 1));
      }
   }
   if (currentPlayer == BLACK)
   {
      if (row - 1 >= 0 and this->board[row - 1][column].getType() == EMPTY) // move one up
      {
         if (row - 1 == 0) // promote
         {
            for (int piece = KNIGHT; piece <= QUEEN; piece++) // consider different promotion options
               moves.push_back(std::make_shared<ChessMove>(row, column, row - 1, column, currentPlayer, piece, -1, -1));
         }
         else
            moves.push_back(std::make_shared<ChessMove>(row, column, row - 1, column, currentPlayer, PAWN, -1, -1));
      }
      if (row == 6 and this->board[row - 1][column].getType() == EMPTY and this->board[row - 2][column].getType() == EMPTY) // move 2 up
      {
         moves.push_back(std::make_shared<ChessMove>(row, column, row - 2, column, currentPlayer, PAWN, -1, -1));
      }
      if (row - 1 >= 0 and column - 1 >= 0 and this->board[row - 1][column - 1].getColor() == WHITE) // take top right
      {
         if (row - 1 == 0) // promote
         {
            for (int piece = KNIGHT; piece <= QUEEN; piece++) // consider different promotion options
               moves.push_back(std::make_shared<ChessMove>(row, column, row - 1, column - 1, currentPlayer, piece, row - 1, column - 1));
         }
         else
            moves.push_back(std::make_shared<ChessMove>(row, column, row - 1, column - 1, currentPlayer, PAWN, row - 1, column - 1));
      }
      if (row - 1 >= 0 and column + 1 < 8 and this->board[row - 1][column + 1].getColor() == WHITE) // take top left
      {
         if (row - 1 == 0) // promote
         {
            for (int piece = KNIGHT; piece <= QUEEN; piece++) // consider different promotion options
               moves.push_back(std::make_shared<ChessMove>(row, column, row - 1, column + 1, currentPlayer, piece, row - 1, column + 1));
         }
         else
            moves.push_back(std::make_shared<ChessMove>(row, column, row - 1, column + 1, currentPlayer, PAWN, row - 1, column + 1));
      }
   }
   return moves;
}

std::vector<std::shared_ptr<ChessMove>> Chess::getKnightMoves(int row, int column)
{
   std::vector<std::pair<int, int>> offsets = {{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}};
   std::vector<std::shared_ptr<ChessMove>> moves;
   for (auto offset : offsets)
   {
      auto potential_move = std::make_pair(offset.first + row, offset.second + column);
      if (potential_move.first < 0 or potential_move.second < 0 or potential_move.first > 7 or potential_move.second > 7) // out of bounds
         continue;
      if (this->board[potential_move.first][potential_move.second].getColor() == currentPlayer) // occupied by our piece
         continue;
      if (this->board[potential_move.first][potential_move.second].getColor() == -1)
      {
         moves.push_back(std::make_shared<ChessMove>(row, column, potential_move.first, potential_move.second, currentPlayer, KNIGHT, -1, -1));
      }
      else
      {
         moves.push_back(std::make_shared<ChessMove>(row, column, potential_move.first, potential_move.second, currentPlayer, KNIGHT, potential_move.first, potential_move.second));
      }
   }
   return moves;
}

std::vector<std::shared_ptr<ChessMove>> Chess::getBishopMoves(int row, int column)
{
   std::vector<std::pair<int, int>> offsets = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
   std::vector<std::shared_ptr<ChessMove>> moves;
   for (auto offset : offsets)
   { // for every direction
      auto new_row = row + offset.first;
      auto new_col = column + offset.second;
      while (new_row >= 0 and new_row < 8 and new_col >= 0 and new_col < 8) // go until blocked/end of board
      {
         if (this->board[new_row][new_col].getColor() == currentPlayer) // blocked by own piece
            break;
         if (this->board[new_row][new_col].getColor() == -1) // empty
            moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, BISHOP, -1, -1));
         else if (this->board[new_row][new_col].getColor() == 1 - currentPlayer) // takes
         {
            moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, BISHOP, new_row, new_col));
            break; // cannot go further
         }
         new_row = new_row + offset.first;
         new_col = new_col + offset.second;
      }
   }
   return moves;
}

std::vector<std::shared_ptr<ChessMove>> Chess::getRookMoves(int row, int column)
{
   std::vector<std::pair<int, int>> offsets = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
   std::vector<std::shared_ptr<ChessMove>> moves;
   for (auto offset : offsets) // for every direction
   {
      auto new_row = row + offset.first;
      auto new_col = column + offset.second;
      while (new_row >= 0 and new_row < 8 and new_col >= 0 and new_col < 8) // go until blocked/end of board
      {
         if (this->board[new_row][new_col].getColor() == currentPlayer) // blocked by own piece
            break;
         if (this->board[new_row][new_col].getColor() == -1) // empty
            moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, ROOK, -1, -1));
         else if (this->board[new_row][new_col].getColor() == 1 - currentPlayer) // takes
         {
            moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, ROOK, new_row, new_col));
            break; // cannot go further
         }
         new_row = new_row + offset.first;
         new_col = new_col + offset.second;
      }
   }
   return moves;
}

std::vector<std::shared_ptr<ChessMove>> Chess::getKingMoves(int row, int column)
{
   std::vector<std::pair<int, int>> offsets = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
   std::vector<std::shared_ptr<ChessMove>> moves;
   for (auto offset : offsets) // for every direction
   {
      auto new_row = row + offset.first;
      auto new_col = column + offset.second;
      if (new_row >= 0 and new_row < 8 and new_col >= 0 and new_col < 8)
      {
         if (this->board[new_row][new_col].getColor() != currentPlayer) // not blocked by own piece
         {
            if (this->board[new_row][new_col].getColor() == -1) // empty
               moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, KING, -1, -1));
            else if (this->board[new_row][new_col].getColor() == 1 - currentPlayer) // takes
               moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, KING, new_row, new_col));
         }
      }
   }
   return moves;
}

std::vector<std::shared_ptr<ChessMove>> Chess::getQueenMoves(int row, int column)
{
   std::vector<std::pair<int, int>> offsets = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
   std::vector<std::shared_ptr<ChessMove>> moves;
   for (auto offset : offsets) // for every direction
   {
      auto new_row = row + offset.first;
      auto new_col = column + offset.second;
      while (new_row >= 0 and new_row < 8 and new_col >= 0 and new_col < 8) // go until blocked/end of board
      {
         if (this->board[new_row][new_col].getColor() == currentPlayer) // blocked by own piece
            break;
         if (this->board[new_row][new_col].getColor() == -1) // empty
            moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, QUEEN, -1, -1));
         else if (this->board[new_row][new_col].getColor() == 1 - currentPlayer) // takes
         {
            moves.push_back(std::make_shared<ChessMove>(row, column, new_row, new_col, currentPlayer, QUEEN, new_row, new_col));
            break; // cannot go further
         }
         new_row = new_row + offset.first;
         new_col = new_col + offset.second;
      }
   }
   return moves;
}

std::set<std::pair<int, int>> BISHOP_OFFSETS = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
std::set<std::pair<int, int>> ROOK_OFFSETS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

bool Chess::isUnderCheck(int color)
{
   std::pair<int, int> start_position = color == BLACK ? this->blackKing : this->whiteKing;
   bool inDanger = false;
   // check for queen move pattern dangers
   std::vector<std::pair<int, int>> offsets = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
   for (auto offset : offsets) // for every direction
   {
      auto new_row = start_position.first + offset.first;
      auto new_col = start_position.second + offset.second;
      while (new_row >= 0 and new_row < 8 and new_col >= 0 and new_col < 8) // go until blocked/end of board
      {
         if (this->board[new_row][new_col].getColor() == color) // own piece in the way
            break;

         if (this->board[new_row][new_col].getColor() == 1 - color) // potential danger
         {
            int dist = std::max(abs(new_row - start_position.first), abs(new_col - start_position.second));
            switch (this->board[new_row][new_col].getType())
            {
            case PAWN:
               if (dist == 1 and BISHOP_OFFSETS.count(offset) > 0)
               {
                  if (color == WHITE and (offset == std::make_pair(1, 1) or offset == std::make_pair(1, -1)))
                     inDanger = true;
                  if (color == BLACK and (offset == std::make_pair(-1, 1) or offset == std::make_pair(-1, -1)))
                     inDanger = true;
               }
               break;
            case KNIGHT:
               break;
            case BISHOP:
               if (BISHOP_OFFSETS.count(offset) > 0)
                  inDanger = true;
               break;
            case ROOK:
               if (ROOK_OFFSETS.count(offset) > 0)
                  inDanger = true;
               break;
            case KING:
               if (dist == 1 and (BISHOP_OFFSETS.count(offset) > 0 or ROOK_OFFSETS.count(offset) > 0))
                  inDanger = true;
               break;
            case QUEEN:
               if (BISHOP_OFFSETS.count(offset) > 0 or ROOK_OFFSETS.count(offset) > 0)
                  inDanger = true;
               break;
            }
            break; // cannot go further
         }
         new_row = new_row + offset.first;
         new_col = new_col + offset.second;
      }
   }
   // check for knight move dangers
   std::vector<std::pair<int, int>> knight_offsets = {{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}};
   for (auto offset : knight_offsets)
   {
      auto new_row = start_position.first + offset.first;
      auto new_col = start_position.second + offset.second;
      if (new_row < 0 or new_col < 0 or new_row > 7 or new_col > 7) // out of bounds
         continue;
      if (this->board[new_row][new_col].getColor() != color and this->board[new_row][new_col].getType() == KNIGHT)
         inDanger = true;
   }
   return inDanger;
}

bool Chess::canPlayerCastle(int color, int side)
{
   if (this->isUnderCheck(color))
      return false;
   std::pair<int, int> king;
   if (color == WHITE)
   {
      if (this->whiteKingMoved)
         return false;
      king = this->whiteKing;
   }
   if (color == BLACK)
   {
      if (this->blackKingMoved)
         return false;
      king = this->blackKing;
   }
   if (side == 0)
   {
      if (color == WHITE and this->whiteKingSideRookMoved)
         return false;
      if (color == BLACK and this->blackKingSideRookMoved)
         return false;

      if (this->board[king.first][king.second - 1].getType() != EMPTY)
         return false;
      if (this->board[king.first][king.second - 2].getType() != EMPTY)
         return false;

      if (this->board[king.first][0].getType() != ROOK or this->board[king.first][0].getColor() != color)
         return false;

      Chess gameCopy(*this); // check if will go over attacked fields
      gameCopy.simulateMove(std::make_shared<ChessMove>(king.first, king.second, king.first, king.second - 1, color, KING, -1, -1));
      if (gameCopy.isUnderCheck(color))
         return false;
      gameCopy.simulateMove(std::make_shared<ChessMove>(king.first, king.second - 1, king.first, king.second - 2, color, KING, -1, -1));
      if (gameCopy.isUnderCheck(color))
         return false;
   }
   if (side == 1)
   {
      if (color == WHITE and this->whiteQueenSideRookMoved)
         return false;
      if (color == BLACK and this->blackQueenSideRookMoved)
         return false;

      if (this->board[king.first][king.second + 1].getType() != EMPTY)
         return false;
      if (this->board[king.first][king.second + 2].getType() != EMPTY)
         return false;
      if (this->board[king.first][7].getType() != ROOK or this->board[king.first][7].getColor() != color)
         return false;

      Chess gameCopy(*this); // check if will go over attacked fields
      gameCopy.simulateMove(std::make_shared<ChessMove>(king.first, king.second, king.first, king.second + 1, color, KING, -1, -1));
      if (gameCopy.isUnderCheck(color))
         return false;
      gameCopy.simulateMove(std::make_shared<ChessMove>(king.first, king.second + 1, king.first, king.second + 2, color, KING, -1, -1));
      if (gameCopy.isUnderCheck(color))
         return false;
   }
   return true;
}

void Chess::handleCastling(bool kingside)
{
   if (kingside)
   {
      auto king = this->currentPlayer == WHITE ? this->whiteKing : this->blackKing;
      this->board[king.first][king.second].setColor(-1);
      this->board[king.first][king.second].setType(Piece::EMPTY);

      this->board[king.first][king.second - 2].setColor(this->currentPlayer);
      this->board[king.first][king.second - 2].setType(KING);

      if (this->currentPlayer == WHITE)
      {
         this->whiteKing = std::make_pair(king.first, king.second - 2);
         this->whiteKingMoved = true;
         this->whiteKingSideRookMoved = true;
      }
      else
      {
         this->blackKing = std::make_pair(king.first, king.second - 2);
         this->blackKingMoved = true;
         this->blackKingSideRookMoved = true;
      }

      // move the rook
      auto rook = this->currentPlayer == WHITE ? std::make_pair(0, 0) : std::make_pair(7, 0);

      this->board[rook.first][rook.second].setColor(-1);
      this->board[rook.first][rook.second].setType(Piece::EMPTY);

      this->board[rook.first][rook.second + 2].setColor(this->currentPlayer);
      this->board[rook.first][rook.second + 2].setType(ROOK);
   }
   else
   {
      auto king = this->currentPlayer == WHITE ? this->whiteKing : this->blackKing;
      this->board[king.first][king.second].setColor(-1);
      this->board[king.first][king.second].setType(Piece::EMPTY);

      this->board[king.first][king.second + 2].setColor(this->currentPlayer);
      this->board[king.first][king.second + 2].setType(KING);

      if (this->currentPlayer == WHITE)
      {
         this->whiteKing = std::make_pair(king.first, king.second + 2);
         this->whiteKingMoved = true;
         this->whiteQueenSideRookMoved = true;
      }
      else
      {
         this->blackKing = std::make_pair(king.first, king.second + 2);
         this->blackKingMoved = true;
         this->blackQueenSideRookMoved = true;
      }

      // move the rook
      auto rook = this->currentPlayer == WHITE ? std::make_pair(0, 7) : std::make_pair(7, 7);

      this->board[rook.first][rook.second].setColor(-1);
      this->board[rook.first][rook.second].setType(Piece::EMPTY);

      this->board[rook.first][rook.second - 3].setColor(this->currentPlayer);
      this->board[rook.first][rook.second - 3].setType(ROOK);
   }
}

std::string Chess::encodePosition()
{
   std::string encoded;
   for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
         encoded += std::to_string(this->board[i][j].getType());
   encoded += std::to_string(this->currentPlayer);
   encoded += std::to_string(this->canPlayerCastle(currentPlayer, 0));
   encoded += std::to_string(this->canPlayerCastle(currentPlayer, 1));
   return encoded;
}

std::string Chess::printBoard()
{
   std::string encoded;
   for (int i = 0; i < 8; i++)
   {
      for (int j = 0; j < 8; j++)
      {
         auto piece = this->board[i][j];
         int name_shift = (piece.getType() != 0) * (piece.getColor() == WHITE ? 0 : 'a' - 'A');
         encoded += PIECE_NAME[piece.getType()] + name_shift;
      }
      encoded += "\n";
   }

   return encoded;
}

// constructor
Chess::Chess() : board(8, std::vector<ChessPiece>(8))
{
   this->currentPlayer = Player::WHITE;
   this->initBoard(this->board);
   this->blackKing = {7, 3};
   this->whiteKing = {0, 3};
   this->whiteKingMoved = false;
   this->blackKingMoved = false;
   this->whiteKingSideRookMoved = false;
   this->whiteQueenSideRookMoved = false;
   this->blackKingSideRookMoved = false;
   this->blackQueenSideRookMoved = false;
}

Chess::Chess(Chess &game)
{
   this->currentPlayer = game.getCurrentPlayer();
   this->board = game.board;
   this->movesHistory = game.movesHistory;
   this->positionHistory = game.positionHistory;
   this->encodedPosition = game.encodedPosition;
   this->blackKing = game.blackKing;
   this->whiteKing = game.whiteKing;
   this->whiteKingMoved = game.whiteKingMoved;
   this->blackKingMoved = game.blackKingMoved;
   this->whiteKingSideRookMoved = game.whiteKingSideRookMoved;
   this->whiteQueenSideRookMoved = game.whiteQueenSideRookMoved;
   this->blackKingSideRookMoved = game.blackKingSideRookMoved;
   this->blackQueenSideRookMoved = game.blackQueenSideRookMoved;
}

Chess::Chess(Chess *game)
{
   this->currentPlayer = game->getCurrentPlayer();
   this->board = game->board;
   this->movesHistory = game->movesHistory;
   this->positionHistory = game->positionHistory;
   this->encodedPosition = game->encodedPosition;
   this->blackKing = game->blackKing;
   this->whiteKing = game->whiteKing;
   this->whiteKingMoved = game->whiteKingMoved;
   this->blackKingMoved = game->blackKingMoved;
   this->whiteKingSideRookMoved = game->whiteKingSideRookMoved;
   this->whiteQueenSideRookMoved = game->whiteQueenSideRookMoved;
   this->blackKingSideRookMoved = game->blackKingSideRookMoved;
   this->blackQueenSideRookMoved = game->blackQueenSideRookMoved;
}

void Chess::test()
{
   std::vector<std::shared_ptr<Move>> possibleMoves = this->getPossibleMoves();
   int count = 0;
   while (this->gameStatus(possibleMoves) == GameStatus::IN_PROGRESS)
   {
      std::cout << "Available: " << std::endl;
      for (unsigned i = 0; i < possibleMoves.size(); i++)
      {
         auto move = std::static_pointer_cast<ChessMove>(possibleMoves[i]);
         std::cout << "move " << i << ": " << move->getFromRow() << " " << move->getFromCol() << " " << move->getToRow() << " " << move->getToCol() << " " << move->getPiece() << " " << move->getColor() << std::endl;
      }
      int index;
      std::cin >> index;
      std::shared_ptr<ChessMove> selectedMove = std::static_pointer_cast<ChessMove>(possibleMoves[index]);
      this->simulateMove(selectedMove);
      this->setCurrentPlayer(1 - this->getCurrentPlayer());
      possibleMoves = this->getPossibleMoves();
      count++;
   }
   std::cout << "Game finished" << std::endl;
}

std::shared_ptr<Game> Chess::clone()
{
   return std::make_shared<Chess>(this);
}

bool Chess::isMoveLegal(std::shared_ptr<ChessMove> cmove)
{
   // positions before potential change
   auto blackKing = this->blackKing;
   auto whiteKing = this->whiteKing;
   ChessPiece taken, origin;

   if (cmove->getPiece() == KING) // update king position
   {
      if (cmove->getColor() == BLACK)
      {
         this->blackKing = std::make_pair(cmove->getToRow(), cmove->getToCol());
      }
      if (cmove->getColor() == WHITE)
      {
         this->whiteKing = std::make_pair(cmove->getToRow(), cmove->getToCol());
      }
   }

   // remember what the piece was before the move (important for promotion moves)
   origin.setColor(cmove->getColor());
   origin.setType(this->board[cmove->getFromRow()][cmove->getFromCol()].getType());

   this->board[cmove->getFromRow()][cmove->getFromCol()].setColor(-1); // clear field where it was
   this->board[cmove->getFromRow()][cmove->getFromCol()].setType(Piece::EMPTY);

   if (cmove->getTakeRow() != -1 and cmove->getTakeCol() != -1) // clear field it took
   {
      // remember what was taken
      taken.setColor(this->board[cmove->getTakeRow()][cmove->getTakeCol()].getColor());
      taken.setType(this->board[cmove->getTakeRow()][cmove->getTakeCol()].getType());

      this->board[cmove->getTakeRow()][cmove->getTakeCol()].setColor(-1);
      this->board[cmove->getTakeRow()][cmove->getTakeCol()].setType(Piece::EMPTY);
   }
   this->board[cmove->getToRow()][cmove->getToCol()].setColor(cmove->getColor());
   this->board[cmove->getToRow()][cmove->getToCol()].setType(cmove->getPiece());

   bool verdict = !this->isUnderCheck(cmove->getColor());
   // undo move here
   this->whiteKing = whiteKing;
   this->blackKing = blackKing;

   this->board[cmove->getToRow()][cmove->getToCol()].setColor(-1);
   this->board[cmove->getToRow()][cmove->getToCol()].setType(Piece::EMPTY);

   if (taken.getColor() != -1)
   {
      this->board[cmove->getTakeRow()][cmove->getTakeCol()].setColor(taken.getColor());
      this->board[cmove->getTakeRow()][cmove->getTakeCol()].setType(taken.getType());
   }

   this->board[cmove->getFromRow()][cmove->getFromCol()].setColor(origin.getColor()); // clear field where it was
   this->board[cmove->getFromRow()][cmove->getFromCol()].setType(origin.getType());

   return verdict;
}

// public

std::vector<std::shared_ptr<Move>> Chess::getPossibleMoves()
{
   std::vector<std::shared_ptr<ChessMove>> potentialMoves;
   // get potential moves
   for (int i = 0; i < 8; i++)
   {
      for (int j = 0; j < 8; j++)
      {
         std::vector<std::shared_ptr<ChessMove>> temp;
         if (this->board[i][j].getColor() != this->getCurrentPlayer())
            continue;
         switch (this->board[i][j].getType())
         {
         case PAWN:
            temp = this->getPawnMoves(i, j);
            break;
         case KNIGHT:
            temp = this->getKnightMoves(i, j);
            break;
         case BISHOP:
            temp = this->getBishopMoves(i, j);
            break;
         case ROOK:
            temp = this->getRookMoves(i, j);
            break;
         case KING:
            temp = this->getKingMoves(i, j);
            break;
         case QUEEN:
            temp = this->getQueenMoves(i, j);
            break;
         }
         potentialMoves.insert(potentialMoves.end(), temp.begin(), temp.end());
      }
   }
   // filter out illegal ones
   std::vector<std::shared_ptr<Move>> legalMoves;
   for (auto move : potentialMoves)
   {
      if (this->isMoveLegal(move))
      {
         legalMoves.push_back(move);
      }
   }
   // add castling moves
   auto king = currentPlayer == WHITE ? this->whiteKing : this->blackKing;
   if (this->canPlayerCastle(currentPlayer, 0))
      legalMoves.push_back(std::make_shared<ChessMove>(king.first, king.second, king.first, king.second - 2, currentPlayer, KING, -1, -1));

   if (this->canPlayerCastle(currentPlayer, 1))
      legalMoves.push_back(std::make_shared<ChessMove>(king.first, king.second, king.first, king.second + 2, currentPlayer, KING, -1, -1));
   return legalMoves;
}

void Chess::simulateMove(std::shared_ptr<Move> move)
{
   auto cmove = std::static_pointer_cast<ChessMove>(move);
   assert(this->getCurrentPlayer() == cmove->getColor());
   this->movesHistory.push_back(cmove);
   if (cmove->getPiece() == KING) // update king position
   {
      if (abs(cmove->getFromCol() - cmove->getToCol()) > 1)
      {
         this->handleCastling(cmove->getFromCol() > cmove->getToCol());
         this->encodedPosition = this->encodePosition();
         this->positionHistory[this->encodedPosition]++;

         return; // everything handled
      }
      if (cmove->getColor() == BLACK)
      {
         this->blackKing = std::make_pair(cmove->getToRow(), cmove->getToCol());
         this->blackKingMoved = true;
      }
      if (cmove->getColor() == WHITE)
      {
         this->whiteKing = std::make_pair(cmove->getToRow(), cmove->getToCol());
         this->whiteKingMoved = true;
      }
   }
   if (cmove->getPiece() == ROOK) // update rook moved
   {
      if (cmove->getColor() == BLACK)
      {
         if (cmove->getFromCol() == 0 and cmove->getFromRow() == 7)
            this->blackKingSideRookMoved = true;
         if (cmove->getFromCol() == 7 and cmove->getFromRow() == 7)
            this->blackQueenSideRookMoved = true;
      }
      if (cmove->getColor() == WHITE)
      {
         if (cmove->getFromCol() == 0 and cmove->getFromRow() == 0)
            this->whiteKingSideRookMoved = true;
         if (cmove->getFromCol() == 7 and cmove->getFromRow() == 0)
            this->whiteQueenSideRookMoved = true;
      }
   }
   this->board[cmove->getFromRow()][cmove->getFromCol()].setColor(-1); // clear field where it was
   this->board[cmove->getFromRow()][cmove->getFromCol()].setType(Piece::EMPTY);
   if (cmove->getTakeRow() != -1 and cmove->getTakeCol() != -1) // clear field it took
   {
      this->board[cmove->getTakeRow()][cmove->getTakeCol()].setColor(-1);
      this->board[cmove->getTakeRow()][cmove->getTakeCol()].setType(Piece::EMPTY);
   }

   this->board[cmove->getToRow()][cmove->getToCol()].setColor(cmove->getColor());
   this->board[cmove->getToRow()][cmove->getToCol()].setType(cmove->getPiece());

   this->encodedPosition = this->encodePosition();
   this->positionHistory[this->encodedPosition]++;
}

int Chess::gameStatus(std::vector<std::shared_ptr<Move>> moves)
{
   if (this->isUnderCheck(currentPlayer) and moves.size() == 0)
      return GameStatus::CHECK_MATE;

   if (!this->isUnderCheck(currentPlayer) and moves.size() == 0)
      return GameStatus::STALE_MATE;

   std::unordered_map<int, int> pieces;
   for (int i = 0; i < 8; i++)
   {
      for (int j = 0; j < 8; j++)
      {
         pieces[this->board[i][j].getType()]++;
      }
   }
   int types_count = pieces.size() - 1; // -1 for empty
   // dead positions
   if (types_count == 1) // king vs king
      return GameStatus::DRAW;
   if (types_count == 2 and pieces[KING] == 2 and pieces[BISHOP] == 1) // king vs king and bishop
      return GameStatus::DRAW;
   if (types_count == 2 and pieces[KING] == 2 and pieces[KNIGHT] == 1) // king vs king and knight
      return GameStatus::DRAW;
   if (this->positionHistory[this->encodedPosition] > 3) // draw by repetition
      return GameStatus::DRAW;

   return GameStatus::IN_PROGRESS;
}

void Chess::run(bool debug)
{
   MCTS mcts(std::make_shared<Chess>(this));
   std::vector<std::shared_ptr<Move>> possibleMoves = this->getPossibleMoves();
   int count = 0;
   while (this->gameStatus(possibleMoves) == GameStatus::IN_PROGRESS)
   {
      mcts.run();
      auto root = mcts.getRoot();
      double bestScore = -1.0;
      std::shared_ptr<ChessMove> selectedMove;
      int chosenChild = 0;
      for (auto son : root->getChildren())
      {
         double score = son.second->getSimulations() == 0 ? 0.0 : (double)(son.second->getScore(this->currentPlayer) / (double)son.second->getSimulations());

         if (score > bestScore)
         {
            selectedMove = std::static_pointer_cast<ChessMove>(root->getPossibleMoves()[son.first]);
            bestScore = score;
            chosenChild = son.first;
         }
      }
      if (debug)
      {
         std::cout << "Available: " << std::endl;
         for (unsigned i = 0; i < possibleMoves.size(); i++)
         {
            auto move = std::static_pointer_cast<ChessMove>(possibleMoves[i]);
            std::cout << move->getFromRow() << " " << move->getFromCol() << " " << move->getToRow() << " " << move->getToCol() << " " << move->getPiece() << " " << move->getColor() << std::endl;
         }
      }
      std::cout << "Chosen: " << std::endl;
      std::cout << selectedMove->getFromRow() << " " << selectedMove->getFromCol() << " " << selectedMove->getToRow() << " " << selectedMove->getToCol() << " " << selectedMove->getPiece() << " " << selectedMove->getColor() << std::endl;

      this->simulateMove(selectedMove);
      this->setCurrentPlayer(1 - this->getCurrentPlayer());
      possibleMoves = this->getPossibleMoves();
      count++;

      if (mcts.getRoot()->getChildren().count(chosenChild) > 0)
         mcts.setRoot(mcts.getRoot()->getChildren()[chosenChild]);
      else
         mcts.setRoot(std::make_shared<Node>(possibleMoves, std::make_shared<Chess>(this), mcts.getRoot()));
   }
   std::cout << "Game finished" << std::endl;
}

const std::vector<std::vector<ChessPiece>> &Chess::getBoard()
{
   return this->board;
}

std::unordered_map<char, int> PIECE_NUMBER = {
    {'N', 2},
    {'B', 3},
    {'R', 4},
    {'Q', 5},
    {'K', 6}};

std::shared_ptr<ChessMove> Chess::getMoveFromStandardNotation(std::string notationMove)
{
   int player = notationMove[0] == 'W' ? 0 : 1;
   assert(player == this->getCurrentPlayer());
   std::vector<std::shared_ptr<Move>> possibleMoves = this->getPossibleMoves();

   std::string actualMove = notationMove.substr(notationMove.find('.') + 1);
   std::vector<char> elements;
   std::shared_ptr<ChessMove> move = nullptr;
   if (actualMove == "O-O-O") // queenside
   {
      for (int i = 0; i < possibleMoves.size(); i++)
      {
         auto mv = std::static_pointer_cast<ChessMove>(possibleMoves[i]);
         if (mv->getPiece() == Piece::KING and abs(mv->getFromCol() - mv->getToCol()) > 1 and mv->getToCol() == 5)
            return mv;
      }
   }
   if (actualMove == "O-O") // kingside
   {
      for (int i = 0; i < possibleMoves.size(); i++)
      {
         auto mv = std::static_pointer_cast<ChessMove>(possibleMoves[i]);
         if (mv->getPiece() == Piece::KING and abs(mv->getFromCol() - mv->getToCol()) > 1 and mv->getToCol() == 1)
            return mv;
      }
   }
   int piece;
   bool takes = false;
   int to_row = -1, to_col = -1;
   int promoted = -1;
   for (int i = actualMove.size() - 1; i >= 0; i--)
   {
      if (actualMove[i] == 'x')
      {
         takes = true;
         continue;
      }
      if (isdigit(actualMove[i]) and to_row == -1 and to_col == -1)
      {
         to_col = 7 - (actualMove[i - 1] - 'a');
         to_row = actualMove[i] - '0' - 1;
      }
      if (actualMove[i] == '=')
         promoted = PIECE_NUMBER[actualMove[i + 1]];
      elements.push_back(actualMove[i]);
   }
   if (islower(actualMove[0]))
   {
      if (promoted == -1)
         piece = Piece::PAWN;
      else
         piece = promoted;
   }
   else
      piece = PIECE_NUMBER[actualMove[0]];
   std::vector<std::shared_ptr<ChessMove>> potentialMoves;
   for (int i = 0; i < possibleMoves.size(); i++)
   {
      std::shared_ptr<ChessMove> move = std::static_pointer_cast<ChessMove>(possibleMoves[i]);
      if (move->getPiece() == piece and move->getColor() == player and move->getToCol() == to_col and move->getToRow() == to_row)
      {
         if (takes and move->getTakeCol() == -1)
            continue;
         potentialMoves.push_back(move);
      }
   }
   assert(potentialMoves.size() > 0);
   if (potentialMoves.size() == 1)
      return potentialMoves[0];
   // std::cout << "Ambigous move" << std::endl;
   int from_col = -1, from_row = -1;
   for (int i = elements.size() - 1; i >= 0; i--)
   {
      if (islower(elements[i]))
      {
         from_col = 7 - (elements[i] - 'a');
         if (isdigit(elements[i - 1]))
            from_row = elements[i - 1] - '0' - 1;
         break;
      }
   }
   for (int i = elements.size() - 1; i >= 0; i--)
   {
      if (isupper(elements[i]))
      {
         if (isdigit(elements[i - 1]))
         {
            from_row = elements[i - 1] - '0' - 1;
            break;
         }
         if (islower(elements[i - 1]))
         {
            from_col = 7 - (elements[i - 1] - 'a');
            if (isdigit(elements[i - 2]))
               from_row = elements[i - 2] - '0' - 1;
            break;
         }
      }
   }
   for (auto move : potentialMoves)
   {
      if (move->getFromCol() == from_col)
      {
         if (from_row == -1 or move->getFromRow() == from_row)
            return move;
      }
      if (move->getFromRow() == from_row)
      {
         if (from_col == -1 or move->getFromCol() == from_col)
            return move;
      }
   }
   std::cerr << "Something went wrong" << std::endl;
   return nullptr;
}