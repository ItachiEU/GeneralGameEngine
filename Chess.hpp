#pragma once
#include "Game.hpp"
#include "ChessPiece.hpp"
#include "ChessMove.hpp"

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
   std::vector<ChessMove *> movesHistory;
   std::unordered_map<std::string, int> positionHistory;
   bool whiteKingMoved;
   bool blackKingMoved;
   bool whiteKingSideRookMoved;
   bool whiteQueenSideRookMoved;
   bool blackKingSideRookMoved;
   bool blackQueenSideRookMoved;
   std::string encodedPosition;

   void initBoard(std::vector<std::vector<ChessPiece>> &board);
   std::vector<ChessMove *> getPawnMoves(int row, int column);
   std::vector<ChessMove *> getKnightMoves(int row, int column);
   std::vector<ChessMove *> getBishopMoves(int row, int column);
   std::vector<ChessMove *> getRookMoves(int row, int column);
   std::vector<ChessMove *> getKingMoves(int row, int column);
   std::vector<ChessMove *> getQueenMoves(int row, int column);
   bool isUnderCheck(int color);
   bool canPlayerCastle(int color, int side);
   void handleCastling(bool kingside);
   std::string encodePosition();

public:
   Chess();
   Chess(Chess &game);
   void run(bool debug = false);
   std::vector<Move *> getPossibleMoves();
   void simulateMove(Move *move);
   int gameStatus(std::vector<Move *> moves);
};