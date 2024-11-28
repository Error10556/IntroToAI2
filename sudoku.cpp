#include "sudoku.h"
#include <cstring>
using namespace std;

Sudoku::DigitReference::DigitReference(Sudoku& owner, int index)
    : owner(owner), index(index)
{
}

int& Sudoku::DigitReference::Ref()
{
    return owner.field[index / 9][index % 9];
}

Sudoku::DigitReference& Sudoku::DigitReference::operator=(int other)
{
    if (other < 0 || other > 9)
        other = 0;
    int& ref = Ref();
    if (ref == other)
        return *this;
    owner.nonzeros -= !!ref;
    int row = index / 9;
    int col = index % 9;
    int blk = Sudoku::BlockNo(row, col);
    owner.errorPairs -= owner.crows[row].ErrorPairs() +
                        owner.ccols[col].ErrorPairs() +
                        owner.cblocks[blk].ErrorPairs();
    owner.excessCounts -= owner.crows[row].ExcessCounts() +
                          owner.ccols[col].ExcessCounts() +
                          owner.cblocks[blk].ExcessCounts();
    owner.crows[row].Dec(ref);
    owner.ccols[col].Dec(ref);
    owner.cblocks[blk].Dec(ref);
    owner.hashval += Sudoku::Powers::Powers[index] * (other - ref);
    ref = other;
    owner.crows[row].Inc(other);
    owner.ccols[col].Inc(other);
    owner.cblocks[blk].Inc(other);
    owner.errorPairs += owner.crows[row].ErrorPairs() +
                        owner.ccols[col].ErrorPairs() +
                        owner.cblocks[blk].ErrorPairs();
    owner.excessCounts += owner.crows[row].ExcessCounts() +
                          owner.ccols[col].ExcessCounts() +
                          owner.cblocks[blk].ExcessCounts();
    owner.nonzeros += !!other;
    return *this;
}

Sudoku::DigitReference::operator int()
{
    return Ref();
}

Sudoku::Sudoku()
{
    for (auto& cnt : crows)
        cnt.Set(0, 9);
    for (auto& cnt : ccols)
        cnt.Set(0, 9);
    for (auto& cnt : cblocks)
        cnt.Set(0, 9);
}

Sudoku::Sudoku(const vector<vector<int>>& f)
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            int src = f[i][j];
            int& dest = field[i][j];
            if (src > 0 && src <= 9)
            {
                dest = src;
                initial[i][j] = true;
            }
            hashval += Powers::Powers[i * 9 + j] * dest;
            crows[i].Inc(dest);
            ccols[j].Inc(dest);
            cblocks[BlockNo(i, j)].Inc(dest);
        }
}

vector<int> Sudoku::Row(int row) const
{
    return {field[row], field[row + 1]};
}

const int* Sudoku::operator[](int row) const
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

constexpr int Sudoku::BlockNo(int row, int col)
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

bitset<9>::reference Sudoku::Initial(int r, int c)
{
    return initial[r][c];
}

bool Sudoku::Initial(int r, int c) const
{
    return initial[r][c];
}

Sudoku::DigitReference Sudoku::Cell(int r, int c)
{
    return DigitReference(*this, r * 9 + c);
}

int Sudoku::Cell(int r, int c) const
{
    return field[r][c];
}

bitset<10> Sudoku::Available(int row, int col) const
{
    return ~(crows[row].NonzeroMap() | ccols[col].NonzeroMap() |
             cblocks[BlockNo(row, col)].NonzeroMap()) &
           ~bitset<10>(1);
}

void Sudoku::FreezeAll()
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            initial[i][j] = !!field[i][j];
}

int Sudoku::ErrorPairCount() const
{
    return errorPairs;
}

int Sudoku::ExcessCounts() const
{
    return excessCounts;
}

int Sudoku::NonzeroCount() const
{
    return nonzeros;
}

int Sudoku::Compare(const Sudoku& other) const
{
    HashValue ha = hashval, hb = other.hashval;
    if (ha < hb)
        return -1;
    if (hb < ha)
        return 1;
    return memcmp(field, other.field, sizeof(field));
}

bool Sudoku::operator<(const Sudoku& other) const
{
    return Compare(other) < 0;
}

bool Sudoku::operator==(const Sudoku& other) const
{
    return Compare(other) == 0;
}

bool Sudoku::operator!=(const Sudoku& other) const
{
    return Compare(other);
}

ostream& operator<<(ostream& out, const Sudoku& s)
{
    for (int i = 0; i < 9; i++)
    {
        const int* row = s[i];
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

istream& operator>>(istream& in, Sudoku& s)
{
    s = Sudoku();
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            char ch;
            in >> ch;
            int a;
            if (ch != '-')
                s.Cell(i, j) = ch - '0';
        }
    }
    s.FreezeAll();
    return in;
}

