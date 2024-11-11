#include <iostream>
#include "sudokusolve.h"
using namespace std;

int main(int argc, char** argv)
{
    int nsol = 1;
    if (argc == 2)
        nsol = atoi(argv[1]);
    Sudoku s;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            char ch; cin >> ch;
            int a;
            if (ch != '-')
                s.Cell(i, j) = ch - '0';
        }
    }
    s.FreezeAll();
    auto res = SolveDFS(s, nsol);
    bool special = false;
    for (int si = 0; si < res.size(); si++)
    {
        auto& sol = res[si];
        if (si)
            cout << '\n';
        for (int i = 0; i < 9; i++)
        {
            const auto& row = sol.Row(i);
            for (int j = 0; j < 9; j++)
            {
                if (j)
                    cout << ' ';
                bool newspec = !sol.Initial(i, j);
                if (newspec != special)
                {
                    cout << "\033[" << (newspec ? "36" : "0") << 'm';
                    special = newspec;
                }
                cout << row[j];
            }
            cout << '\n';
        }
    }
    if (special)
        cout << "\033[0m";
}
