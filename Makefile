sudoku.o: sudoku.cpp
	clang++ -std=c++11 -O2 sudoku.cpp -o sudoku.o

sudokusolve.o: sudokusolve.cpp
	clang++ -std=c++11 -O2 sudokusolve.cpp -o sudokusolve.o
