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
   std::vector<std::shared_ptr<ChessMove>> movesHistory;
   std::unordered_map<std::string, int> positionHistory;
   bool whiteKingMoved;
   bool blackKingMoved;
   bool whiteKingSideRookMoved;
   bool whiteQueenSideRookMoved;
   bool blackKingSideRookMoved;
   bool blackQueenSideRookMoved;
   std::string encodedPosition;

   void initBoard(std::vector<std::vector<ChessPiece>> &board);
   std::vector<std::shared_ptr<ChessMove>> getPawnMoves(int row, int column);
   std::vector<std::shared_ptr<ChessMove>> getKnightMoves(int row, int column);
   std::vector<std::shared_ptr<ChessMove>> getBishopMoves(int row, int column);
   std::vector<std::shared_ptr<ChessMove>> getRookMoves(int row, int column);
   std::vector<std::shared_ptr<ChessMove>> getKingMoves(int row, int column);
   std::vector<std::shared_ptr<ChessMove>> getQueenMoves(int row, int column);
   bool isMoveLegal(std::shared_ptr<ChessMove> move);
   bool isUnderCheck(int color);
   bool canPlayerCastle(int color, int side);
   void handleCastling(bool kingside);
   std::string encodePosition();

public:
   Chess();
   Chess(Chess &game);
   Chess(Chess *game);
   void test();
   void run(bool debug = false);
   std::vector<std::shared_ptr<Move>> getPossibleMoves();
   void simulateMove(std::shared_ptr<Move> move);
   int gameStatus(std::vector<std::shared_ptr<Move>> moves);
   std::shared_ptr<Game> clone();
   const std::vector<std::vector<ChessPiece>> &getBoard();
   std::shared_ptr<ChessMove> moveFromLANNotation(std::string notationMove = "e2e4");
   std::string LANfromMove(std::shared_ptr<ChessMove> chessMove);
   std::shared_ptr<Move> getMoveFromStandardNotation(std::string notationMove = "W1.d4");
   std::string printBoard();
};