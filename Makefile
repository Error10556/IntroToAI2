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

difficulty.bin: difficulty.cpp sudoku.o sudoku.h
	clang++ -std=c++20 -O2 difficulty.cpp sudoku.o -o difficulty.bin

launcher.o: launcher.cpp
	clang++ -std=c++20 -O2 -c launcher.cpp -o launcher.o

benchmark.bin: launcher.o launcher.h benchmark.cpp
	clang++ -std=c++20 -O2 launcher.o benchmark.cpp -o benchmark.bin

clean:
	rm *.o *.bin
