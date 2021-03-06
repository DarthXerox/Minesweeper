CXXFLAGS = -Wall -std=c++11 -Wextra
all: clean minesweeper

minesweeper: main.o Tile.o Minefield.o MinesweeperAI.o
	g++ main.o Tile.o Minefield.o -o Minesweeper -lsfml-graphics -lsfml-window -lsfml-system

main.o : main.cpp
Tile.o : Tile.h
Minefield.o : Minefield.h
MinesweeperAI.o : MinesweeperAI.h

run:
	make clean 
	make all

clean:
	$(RM) *.o Minesweeper 

.PHONY: clean all
