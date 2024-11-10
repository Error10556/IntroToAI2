#pragma once
#include <vector>
#include "sudoku.h"

std::vector<Sudoku> SolveDFS(Sudoku s, int maxresults);
bool SolveDFS(Sudoku& s);
