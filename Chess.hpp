#pragma once
#include "Game.hpp"
#include "ChessPiece.hpp"
#include "Move.hpp"

enum GameStatus
{
   IN_PROGRESS = -1,
   CHECK_MATE = 0,
   STALE_MATE = 1,
   DRAW = 2
};

class Chess : public Game
{
private:
   std::vector<std::vector<ChessPiece>> board;
   std::pair<int, int> whiteKing;
   std::pair<int, int> blackKing;
   std::vector<Move> movesHistory;
   std::unordered_map<std::string, int> positionHistory;
   bool whiteKingMoved;
   bool blackKingMoved;
   bool whiteKingSideRookMoved;
   bool whiteQueenSideRookMoved;
   bool blackKingSideRookMoved;
   bool blackQueenSideRookMoved;
   std::string encodedPosition;

   void initBoard(std::vector<std::vector<ChessPiece>> &board);
   std::vector<Move> getPawnMoves(int row, int column);
   std::vector<Move> getKnightMoves(int row, int column);
   std::vector<Move> getBishopMoves(int row, int column);
   std::vector<Move> getRookMoves(int row, int column);
   std::vector<Move> getKingMoves(int row, int column);
   std::vector<Move> getQueenMoves(int row, int column);
   bool isUnderCheck(int color);
   bool canPlayerCastle(int color, int side);
   void handleCastling(bool kingside);
   std::string encodePosition();

public:
   Chess();
   Chess(Chess &game);
   void run();
   std::vector<Move> getPossibleMoves();
   void simulateMove(Move move);
   int gameStatus(std::vector<Move> moves);
};