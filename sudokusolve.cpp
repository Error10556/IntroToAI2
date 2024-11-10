#include "sudokusolve.h"
#include "sudoku.h"
using namespace std;

void NextUnknown(const Sudoku& s, int& x, int& y)
{
    do
    {
        x++;
        if (x == 9)
            y++, x = 0;
    } while (y < 9 && s.Cell(y, x) != 0);
}

void DFS(Sudoku& cur, vector<Sudoku>& sols, int x, int y, int maxsols,
         ShuffleProc shuffler, void* shArgs)
{
    if (y >= 9)
    {
        sols.push_back(cur);
        return;
    }
    int nx = x, ny = y;
    NextUnknown(cur, nx, ny);
    auto avail = cur.Available(y, x);
    auto cell = cur.Cell(y, x);
    vector<int> digs;
    for (int i = 1; i <= 9; i++)
    {
        if (avail.test(i))
            digs.push_back(i);
    }
    shuffler(digs, shArgs);
    for (int i : digs)
    {
        cell = i;
        DFS(cur, sols, nx, ny, maxsols, shuffler, shArgs);
        if (sols.size() == maxsols)
            break;
    }
    cell = 0;
    return;
}

std::vector<Sudoku> SolveDFS(const Sudoku& s, int maxresults)
{
    return SolveDFS(s, maxresults, [](vector<int>&, void*){}, nullptr);
}

std::vector<Sudoku> SolveDFS(Sudoku s, int maxresults, ShuffleProc shuffler,
                             void* shuffleArgs)
{
    int x, y;
    NextUnknown(s, x, y);
    vector<Sudoku> res;
    DFS(s, res, x, y, maxresults, shuffler, shuffleArgs);
    return res;
}

bool SolveDFS(Sudoku& s)
{
    auto res = SolveDFS(s, 1);
    if (res.empty())
        return false;
    s = res[0];
    return true;
}
