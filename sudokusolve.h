#pragma once
#include "sudoku.h"
#include <vector>

typedef void (*ShuffleProc)(std::vector<int>& digits, void* args);

std::vector<Sudoku> SolveDFS(const Sudoku& s, int maxresults);
std::vector<Sudoku> SolveDFS(Sudoku s, int maxresults, ShuffleProc shuffler,
                             void* shufflerArgs);
bool SolveDFS(Sudoku& s);
