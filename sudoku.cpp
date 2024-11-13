#include "sudoku.h"
using namespace std;

Sudoku::DigitReference::DigitReference(int& ref) : ref(ref)
{
}

Sudoku::DigitReference& Sudoku::DigitReference::operator=(int other)
{
    if (other < 0 || other > 9)
        other = 0;
    ref = other;
    return *this;
}

Sudoku::DigitReference::operator int()
{
    return ref;
}

Sudoku::Sudoku() : field(9, vector<int>(9)), initial(9, vector<bool>(9))
{
}

Sudoku::Sudoku(const vector<vector<int>>& f)
    : field(9, vector<int>(9)), initial(9, vector<bool>(9))
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            int src = f[i][j];
            if (src <= 0 || src > 9)
                continue;
            field[i][j] = src;
            initial[i][j] = true;
        }
}

const vector<int>& Sudoku::Row(int row) const
{
    return field[row];
}

const vector<int>& Sudoku::operator[](int row) const
{
    return field[row];
}

vector<int> Sudoku::Column(int col) const
{
    vector<int> res(9);
    for (int i = 0; i < 9; i++)
        res[i] = field[i][col];
    return res;
}

void Sudoku::Block(int no, int& xstart, int& xend, int& ystart, int& yend)
{
    xstart = no % 3 * 3;
    xend = xstart + 3;
    ystart = no / 3 * 3;
    yend = ystart + 3;
}

int Sudoku::BlockNo(int row, int col)
{
    return row / 3 * 3 + col / 3;
}

vector<int> Sudoku::Block(int no) const
{
    vector<int> res;
    res.reserve(9);
    int xstart, xend, ystart, yend;
    Block(no, xstart, xend, ystart, yend);
    for (int i = ystart; i < yend; i++)
        for (int j = xstart; j < xend; j++)
            res.push_back(field[i][j]);
    return res;
}

_Bit_reference Sudoku::Initial(int r, int c)
{
    return initial[r][c];
}

bool Sudoku::Initial(int r, int c) const
{
    return initial[r][c];
}

Sudoku::DigitReference Sudoku::Cell(int r, int c)
{
    return DigitReference(field[r][c]);
}

int Sudoku::Cell(int r, int c) const
{
    return field[r][c];
}

bitset<10> Sudoku::Available(int row, int col) const
{
    bitset<10> res;
    res.set();
    res.reset(0);
    for (int i : field[row])
        res.reset(i);
    for (int i = 0; i < 9; i++)
        res.reset(field[i][col]);
    int blk = BlockNo(row, col);
    int xstart, xend, ystart, yend;
    Block(blk, xstart, xend, ystart, yend);
    for (int i = ystart; i < yend; i++)
        for (int j = xstart; j < xend; j++)
            res.reset(field[i][j]);
    return res;
}

void Sudoku::FreezeAll()
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            initial[i][j] = !!field[i][j];
}
