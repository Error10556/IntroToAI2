#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>
#include <vector>
using namespace std;

class Sudoku
{
public:
    struct DigitReference
    {
    private:
        int& ref;

    public:
        DigitReference(int& ref);
        int& operator=(int val);
        operator int();
    };

private:
    std::vector<std::vector<int>> field;
    std::vector<std::vector<bool>> initial;

public:
    static int BlockNo(int row, int col);
    static void Block(int no, int& xstart, int& xend, int& ystart, int& yend);
    Sudoku();
    Sudoku(const std::vector<std::vector<int>>& field);
    const std::vector<int>& Row(int row) const;
    std::vector<int> Column(int col) const;
    std::vector<int> Block(int no) const;
    std::_Bit_reference Initial(int r, int c);
    bool Initial(int r, int c) const;
    DigitReference Cell(int r, int c);
    int Cell(int r, int c) const;
    const std::vector<int>& operator[](int row) const;
    std::bitset<10> Available(int row, int col) const;
    void FreezeAll();
};

Sudoku::DigitReference::DigitReference(int& ref) : ref(ref)
{
}

int& Sudoku::DigitReference::operator=(int other)
{
    if (other < 0 || other > 9)
        other = 0;
    ref = other;
    return ref;
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

template <class Iter> void DigitCounts(Iter begin, Iter end, int counts[10])
{
    while (begin != end)
    {
        counts[*begin]++;
        ++begin;
    }
}

class Chromosome
{
public:
    Sudoku Field;

private:
    typedef bool (*CoordFilter)(const Sudoku& field, int y, int x);
    vector<pair<int, int>> AllCoords(CoordFilter filter)
    {
        vector<pair<int, int>> res;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (filter(Field, i, j))
                    res.emplace_back(i, j);
        return res;
    }
    vector<pair<int, int>> AllNonInitialCoords()
    {
        return AllCoords([](const Sudoku& field, int y, int x) {
            return !field.Initial(y, x);
        });
    }
    vector<pair<int, int>> AllEmptyCoords()
    {
        return AllCoords(
            [](const Sudoku& field, int y, int x) { return !field[y][x]; });
    }
    vector<pair<int, int>> AllNonInitialFilledCoords()
    {
        return AllCoords([](const Sudoku& field, int y, int x) {
            return !field.Initial(y, x) && field[y][x];
        });
    }

public:
    static constexpr int MaxFitness = 81;
    // A number in [-891, 81] (where 81 = Chromosome::MaxFitness)
    // The empty sudoku has fitness 0
    int Fitness() const
    {
        const int maxfit = 81;
        int fit = 0;
        for (int i = 0; i < 9; i++)
            for (int j : Field[i])
                fit += !!j;
        auto Punish = [&fit](const vector<int>& v) -> void {
            int digitcounts[10]{};
            DigitCounts(v.data(), v.data() + 9, digitcounts);
            for (int j = 1; j <= 9; j++)
                fit -= j * (j - 1) / 2;
        };
        for (int i = 0; i < 9; i++)
        {
            Punish(Field[i]);
            Punish(Field.Column(i));
            Punish(Field.Block(i));
        }
        return fit;
    }
    bool MutateGrow()
    {
        pair<int, int> p;
        bitset<10> avail;
        auto empty = AllEmptyCoords();
        while (empty.size())
        {
            int ind = rand() % empty.size();
            p = empty[ind];
            avail = Field.Available(p.first, p.second);
            if (avail.none())
            {
                empty[ind] = empty.back();
                empty.pop_back();
                continue;
            }
            break;
        }
        if (empty.empty())
            return false;
        vector<int> digits;
        for (int i = 1; i <= 9; i++)
            if (avail.test(i))
                digits.push_back(i);
        Field.Cell(p.first, p.second) = digits[rand() % digits.size()];
        return true;
    }
    bool MutateRemove()
    {
        auto nonempty = AllNonInitialFilledCoords();
        if (nonempty.empty())
            return false;
        auto p = nonempty[rand() % nonempty.size()];
        Field.Cell(p.first, p.second) = 0;
        return true;
    }
    bool MutateChange()
    {
        auto nonempty = AllNonInitialFilledCoords();
        while (nonempty.size())
        {
            int ind = rand() % nonempty.size();
            auto p = nonempty[ind];
            auto cell = Field.Cell(p.first, p.second);
            int val = cell;
            cell = 0;
            auto avail = Field.Available(p.first, p.second);
            avail.reset(val);
            if (avail.none())
            {
                nonempty[ind] = nonempty.back();
                nonempty.pop_back();
                cell = val;
                continue;
            }
            vector<int> av;
            for (int i = 1; i <= 9; i++)
                if (avail.test(i))
                    av.push_back(i);
            cell = av[rand() % av.size()];
            return true;
        }
        return false;
    }
    bool MutateSwapInRegion(vector<pair<int, int>> region)
    {
        vector<pair<int, int>> swappable;
        for (auto p : region)
            if (!Field.Initial(p.first, p.second) && Field[p.first][p.second])
                swappable.push_back(p);
        int n = swappable.size();
        if (n < 2)
            return false;
        int a = rand() % n;
        int b = rand() % (n - 1);
        b += b >= a;
        auto pa = swappable[a];
        auto pb = swappable[b];
        int buf = Field[pa.first][pa.second];
        Field.Cell(pa.first, pa.second) = Field[pb.first][pb.second];
        Field.Cell(pb.first, pb.second) = buf;
        return true;
    }
    bool MutateSwapInRow()
    {
        vector<int> rows(9);
        iota(rows.begin(), rows.end(), 0);
        while (rows.size())
        {
            int ind = rand() % rows.size();
            int row = rows[ind];
            vector<pair<int, int>> region(9);
            for (int i = 0; i < 9; i++)
                region[i] = {row, i};
            if (MutateSwapInRegion(region))
                return true;
            rows[ind] = rows.back();
            rows.pop_back();
        }
        return false;
    }
    bool MutateSwapInColumn()
    {
        vector<int> cols(9);
        iota(cols.begin(), cols.end(), 0);
        while (cols.size())
        {
            int ind = rand() % cols.size();
            int col = cols[ind];
            vector<pair<int, int>> region(9);
            for (int i = 0; i < 9; i++)
                region[i] = {i, col};
            if (MutateSwapInRegion(region))
                return true;
            cols[ind] = cols.back();
            cols.pop_back();
        }
        return false;
    }
    bool MutateSwapInBlock()
    {
        vector<int> blocks(9);
        iota(blocks.begin(), blocks.end(), 0);
        while (blocks.size())
        {
            int ind = rand() % blocks.size();
            int block = blocks[ind];
            vector<pair<int, int>> region;
            int xstart, xend, ystart, yend;
            Sudoku::Block(block, xstart, xend, ystart, yend);
            for (int i = ystart; i < yend; i++)
                for (int j = xstart; j < xend; j++)
                    region.emplace_back(i, j);
            if (MutateSwapInRegion(region))
                return true;
            blocks[ind] = blocks.back();
            blocks.pop_back();
        }
        return false;
    }
    bool MutateSwap()
    {
        vector<int> modes{0, 1, 2};
        swap(modes[0], modes[rand() % 3]);
        swap(modes[1], modes[rand() % 2 + 1]);
        for (int mode : modes)
            switch (mode)
            {
            case 0:
                if (MutateSwapInRow())
                    return true;
            case 1:
                if (MutateSwapInColumn())
                    return true;
            case 2:
                if (MutateSwapInBlock())
                    return true;
            }
        return false;
    }
    void Mutate()
    {
        int modes[]{0, 1, 2, 3}; // grow, remove, change, swap
        for (int i = 0; i < 3; i++)
            swap(modes[i],
                 modes[rand() % (sizeof(modes) / sizeof(int) - i) + i]);
        for (int mode : modes)
            switch (mode)
            {
            case 0:
                if (MutateGrow())
                    return;
            case 1:
                if (MutateRemove())
                    return;
            case 2:
                if (MutateChange())
                    return;
            case 3:
                if (MutateSwap())
                    return;
            }
    }
    void ColumnCrossover(const Chromosome& other)
    {

    }
    void RowCrossover(const Chromosome& other)
    {
    }
    void BlockCrossover(const Chromosome& other)
    {
    }
    void Crossover(const Chromosome& other)
    {
        switch (rand() % 3)
        {
        case 0:
            RowCrossover(other);
            break;
        case 1:
            ColumnCrossover(other);
            break;
        case 2:
            BlockCrossover(other);
            break;
        }
    }
};

class Population
{
};

int main()
{
    srand(clock());
}
