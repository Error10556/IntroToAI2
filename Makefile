sudoku.o: sudoku.cpp
	clang++ -std=c++20 -O2 -c sudoku.cpp -o sudoku.o

sudokusolve.o: sudokusolve.cpp
	clang++ -std=c++20 -O2 -c sudokusolve.cpp -o sudokusolve.o

testgen.bin: testgen.cpp sudokusolve.h sudoku.h sudokusolve.o sudoku.o
	clang++ -std=c++20 -O2 testgen.cpp sudokusolve.o sudoku.o -o testgen.bin

dfssolver.bin: dfssolver.cpp sudokusolve.h sudoku.h sudokusolve.o sudoku.o
	clang++ -std=c++20 -O2 dfssolver.cpp sudokusolve.o sudoku.o -o dfssolver.bin

genetic.bin: genetic.cpp
	clang++ -std=c++20 -O2 genetic.cpp -o genetic.bin

clean:
	rm *.o *.bin
