sudoku.o: sudoku.cpp
	clang++ -std=c++11 -O2 -c sudoku.cpp -o sudoku.o

sudokusolve.o: sudokusolve.cpp
	clang++ -std=c++11 -O2 -c sudokusolve.cpp -o sudokusolve.o

testgen.o: testgen.cpp sudokusolve.h sudoku.h
	clang++ -std=c++11 -O2 -c testgen.cpp -o testgen.o

testgen.bin: testgen.o sudokusolve.o sudoku.o
	clang++ -O2 testgen.o sudokusolve.o sudoku.o -o testgen.bin

clean:
	rm *.o *.bin
