#include <bits/stdc++.h>
#include "sudoku.h"
using namespace std;

int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        cout << "Usage: difficulty [--verbose] mytest.txt" << endl;
        return 1;
    }
    int fileindex = 1;
    bool verbose = false;
    if (strcmp(argv[fileindex], "--verbose") == 0)
    {
        fileindex++;
        verbose = true;
    }
    ifstream fs(argv[fileindex]);
    Sudoku sd;
    fs >> sd;
    fs.close();
    int before = sd.NonzeroCount();
    while (true)
    {
        bool ok = false;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
            {
                if (sd[i][j])
                    continue;
                auto cell = sd.Cell(i, j);
                bitset<10> avail = sd.Available(i, j);
                if (avail.count() != 1)
                    continue;
                cell = avail._Find_first();
                ok = true;
            }
        if (!ok)
            break;
    }
    int after = sd.NonzeroCount();
    if (verbose)
    {
        cout << "Initially:           " << before << endl;
        cout << "After trivial steps: " << after << endl;
    }
    cout << 81 - (before + 2 * after) / 3 << endl;
}
