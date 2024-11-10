#include "sudoku.h"
#include "sudokusolve.h"
#include <climits>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
using namespace std;

FILE* randgen;

int RandInt()
{
    int res;
    fread(&res, sizeof(int), 1, randgen);
    res &= INT_MAX;
    return res;
}

int RandInt(int n)
{
    return RandInt() % n;
}

int RandInt(int min, int exmax)
{
    return RandInt(exmax - min) + min;
}

template <class T> void Shuffle(vector<T>& v, void*)
{
    int n = v.size();
    for (int i = 0; i < n - 1; i++)
        swap(v[i], v[RandInt(i, n)]);
}

Sudoku RandomSudoku()
{
    Sudoku empty;
    auto v = SolveDFS(empty, 1, Shuffle<int>, nullptr);
    return v[0];
}

bool SingleSol(const Sudoku& s)
{
    auto res = SolveDFS(s, 2);
    return res.size() == 1;
}

void StripSudoku(Sudoku& s)
{
    vector<pair<int, int>> coords(81);
    for (int i = coords.size() - 1; i >= 0; i--)
        coords[i] = {i / 9, i % 9};
    Shuffle(coords, nullptr);
    for (auto& p : coords)
    {
        auto cell = s.Cell(p.first, p.second);
        int val = cell;
        cell = 0;
        if (SingleSol(s))
            continue;
        cell = val;
        break;
    }
}

int NumberOfDigits(const Sudoku& s)
{
    int res = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            res += !!s[i][j];
    return res;
}

void RestoreDigits(Sudoku& s, const Sudoku& solution, int noDigits)
{
    vector<pair<int, int>> coords(81);
    for (int i = coords.size() - 1; i >= 0; i--)
        coords[i] = {i / 9, i % 9};
    Shuffle(coords, nullptr);
    for (auto p : coords)
    {
        auto cell = s.Cell(p.first, p.second);
        if (!!cell)
            continue;
        cell = solution[p.first][p.second];
        if (!--noDigits)
            break;
    }
}

ostream& operator<<(ostream& out, const Sudoku& s)
{
    for (int i = 0; i < 9; i++)
    {
        auto& row = s.Row(i);
        for (int j = 0; j < 9; j++)
        {
            if (j)
                out << ' ';
            if (row[j])
                out << row[j];
            else
                out << '-';
        }
        out << '\n';
    }
    return out;
}

string MakeTestName(const string& dir, int noDigits, int testindex, int wDigits,
                    int wIndex)
{
    stringstream fnstream;
    fnstream << dir << "/test";
    fnstream.fill('0');
    fnstream.width(wDigits);
    fnstream << noDigits << '-';
    fnstream.width(wIndex);
    fnstream << testindex;
    return fnstream.str();
}

int main(int argc, char** argv)
{
    randgen = fopen("/dev/random", "r");
    if (argc == 1)
    {
        cout << "Usage: testgen directory #OfTestsPerGroup startcount "
                "endcount\n";
        return 0;
    }
    string dir = argv[1];
    while (dir.back() == '/')
        dir.pop_back();
    mkdir(dir.c_str(), 0);
    int noTests = atoi(argv[2]);
    int wIndex = strlen(argv[2]);
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    int wDigits = strlen(argv[4]);
    map<int, int> left;
    for (int i = start; i <= end; i++)
        left[i] = noTests;

    int progress = 0, maxProgress = noTests * (end - start + 1);
    while (left.size())
    {
        Sudoku sol = RandomSudoku();
        Sudoku s;
        int noDigits;
        do
        {
            s = sol;
            StripSudoku(s);
            noDigits = NumberOfDigits(s);
        } while (noDigits > left.rbegin()->first);
        auto iter = left.lower_bound(noDigits);
        int needDigits = iter->first;
        if (needDigits > noDigits)
        {
            RestoreDigits(s, sol, needDigits - noDigits);
            noDigits = needDigits;
        }
        iter->second--;
        string filename = MakeTestName(dir, noDigits, noTests - iter->second,
                                       wDigits, wIndex);
        ofstream(filename, ios::out) << s;
        if (!iter->second)
            left.erase(iter);
        progress++;
        cout << "\r" << progress * 100 / maxProgress << '%';
    }
    cout << "\nDone\n";
    fclose(randgen);
}
