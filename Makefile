PROG = game-engine
CC = g++
CPPFLAGS = -O2 -std=c++17 -Wall -g
OBJS = main.o Move.o ChessMove.o Game.o ChessPiece.o Chess.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS)

main.o :
	$(CC) -c $(CPPFLAGS) main.cpp
Game.o:
	$(CC) -c $(CPPFLAGS) Game.cpp
Move.o:
	$(CC) -c $(CPPFLAGS) Move.cpp
ChesMove.o:
	$(CC) -c $(CPPFLAGS) ChesMove.cpp
Chess.o:
	$(CC) -c $(CPPFLAGS) Chess.cpp
ChessPiece.o:
	$(CC) -c $(CPPFLAGS) ChessPiece.cpp
clean :
	rm -f core $(PROG) $(OBJS)