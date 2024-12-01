## Sudoku solver
Everything is made by me.

### Makefile targets
1. testgen.bin
2. dfssolver.bin
3. genetic.bin
4. difficulty.bin
5. benchmark.bin
6. givens.bin
7. reporter.bin

### How to use
For any binary file, except `genetic.bin`, run it without arguments to see how to use it.

#### testgen.bin
Generates testcases.

#### dfssolver.bin
Solves a testcase using a simple recursive algorithm.

#### genetic.bin
Solves a sudoku from `stdin`. Accepts flags: `--verbose` (makes it give updates on its progress), `--report` (makes it show a report at the end).

#### difficulty.bin
Measures the difficulty of a testcase as a linear combination of the number of initially unknown cells and the number of unknown cells after filling in trivial cells (cells whose contents are obvious). Unused.

#### benchmark.bin
Measures the runtime of a solution. Unused.

#### givens.bin
Outputs the number of hyphens in a text file.

#### reporter.bin
Generates report data as per the assignment.
