CXXFLAGS = -Wall -std=c++11 -Wextra
all: clean minesweeper cleanO

minesweeper: main.o Tile.o Minefield.o MinesweeperAI.o
	g++ main.o Tile.o Minefield.o -o Minesweeper -lsfml-graphics -lsfml-window -lsfml-system

main.o : main.cpp
Tile.o : Tile.h
Minefield.o : Minefield.h
MinesweeperAI.o : MinesweeperAI.h

clean:
	$(RM) *.o Minesweeper

cleanO:
	$(RM) *.o

.PHONY: clean all
