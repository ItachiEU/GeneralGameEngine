PROG = game-engine
CC = g++
CPPFLAGS = -O2 -std=c++17 -Wall -g -fsanitize=address
LDFLAGS=-lm -fsanitize=address -g
OBJS = main.o Move.o ChessMove.o Game.o ChessPiece.o Chess.o Node.o MCTS.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS)

main.o :
	$(CC) -c $(CPPFLAGS) main.cpp $(LDFLAGS)
Game.o:
	$(CC) -c $(CPPFLAGS) Game.cpp $(LDFLAGS)
Move.o:
	$(CC) -c $(CPPFLAGS) Move.cpp $(LDFLAGS)
ChesMove.o:
	$(CC) -c $(CPPFLAGS) ChesMove.cpp $(LDFLAGS)
Chess.o:
	$(CC) -c $(CPPFLAGS) Chess.cpp $(LDFLAGS)
ChessPiece.o:
	$(CC) -c $(CPPFLAGS) ChessPiece.cpp $(LDFLAGS)
Node.o:
	$(CC) -c $(CPPFLAGS) Node.cpp $(LDFLAGS)
MCTS.o:
	$(CC) -c $(CPPFLAGS) MCTS.cpp $(LDFLAGS)
clean :
	rm -f core $(PROG) $(OBJS)