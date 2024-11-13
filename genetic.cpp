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
        DigitReference& operator=(int val);
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

int Choose2(int n)
{
    return n * (n - 1) / 2;
}

class Chromosome
{
private:
    Sudoku field;
    typedef bool (*CoordFilter)(const Sudoku& field, int y, int x);
    vector<pair<int, int>> AllCoords(CoordFilter filter)
    {
        vector<pair<int, int>> res;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (filter(field, i, j))
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
    void SetDigitCount(int& digitcount, int val)
    {
        errorpairs -= Choose2(digitcount);
        errorpairs += Choose2(val);
        digitcount = val;
    }
    void IncreaseDigitCount(int& digitcount)
    {
        errorpairs += digitcount++;
    }
    void DecreaseDigitCount(int& digitcount)
    {
        errorpairs -= --digitcount;
    }
    vector<vector<int>> rowcounts, colcounts, blockcounts;
    int nonzeros, errorpairs;

    static void RandomMutationIndices(int& no, int inds[9])
    {
        no = rand() % 8 + 1;
        iota(inds, inds + 9, 0);
        for (int i = 0; i < no; i++)
            swap(inds[i], inds[i + rand() % (9 - i)]);
    }

public:
    struct DigitReference
    {
    private:
        Chromosome* owner;
        int x, y;

    public:
        DigitReference(Chromosome* owner, int y, int x)
            : owner(owner), x(x), y(y)
        {
        }
        operator int() const
        {
            return owner->field[y][x];
        }
        DigitReference& operator=(int val)
        {
            if (val < 0 || val > 9)
                val = 0;
            auto cell = owner->field.Cell(y, x);
            int prv = cell;
            int blk = Sudoku::BlockNo(y, x);
            if (prv != 0)
            {
                owner->DecreaseDigitCount(owner->rowcounts[y][prv]);
                owner->DecreaseDigitCount(owner->colcounts[x][prv]);
                owner->DecreaseDigitCount(owner->blockcounts[blk][prv]);
                owner->nonzeros--;
            }
            if (val != 0)
            {
                owner->IncreaseDigitCount(owner->rowcounts[y][val]);
                owner->IncreaseDigitCount(owner->colcounts[x][val]);
                owner->IncreaseDigitCount(owner->blockcounts[blk][val]);
                owner->nonzeros++;
            }
            return *this;
        }
    };
    Chromosome(const Sudoku& f)
        : field(f), rowcounts(9, vector<int>(10)), nonzeros(0), errorpairs(0)
    {
        colcounts = blockcounts = rowcounts;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
            {
                int dig = field[i][j];
                if (!dig)
                    continue;
                nonzeros++;
                rowcounts[i][dig]++;
                colcounts[j][dig]++;
                blockcounts[Sudoku::BlockNo(i, j)][dig]++;
            }
        for (const auto& v : {rowcounts, colcounts, blockcounts})
            for (const vector<int>& cnts : v)
                for (int cnt : cnts)
                    errorpairs += Choose2(cnt);
    }
    const Sudoku& Field() const
    {
        return field;
    }
    DigitReference Cell(int row, int col)
    {
        return DigitReference(this, row, col);
    }
    static constexpr int MaxFitness = 81;
    // A number in [-891, 81] (where 81 = Chromosome::MaxFitness)
    // The empty sudoku has fitness 0
    int Fitness() const
    {
        return nonzeros - errorpairs;
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
            avail = field.Available(p.first, p.second);
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
        Cell(p.first, p.second) = digits[rand() % digits.size()];
        return true;
    }
    bool MutateRemove()
    {
        auto nonempty = AllNonInitialFilledCoords();
        if (nonempty.empty())
            return false;
        auto p = nonempty[rand() % nonempty.size()];
        Cell(p.first, p.second) = 0;
        return true;
    }
    bool MutateChange()
    {
        auto nonempty = AllNonInitialFilledCoords();
        while (nonempty.size())
        {
            int ind = rand() % nonempty.size();
            auto p = nonempty[ind];
            auto cell = Cell(p.first, p.second);
            int val = cell;
            cell = 0;
            auto avail = field.Available(p.first, p.second);
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
            if (!field.Initial(p.first, p.second) && field[p.first][p.second])
                swappable.push_back(p);
        int n = swappable.size();
        if (n < 2)
            return false;
        int a = rand() % n;
        int b = rand() % (n - 1);
        b += b >= a;
        auto pa = swappable[a];
        auto pb = swappable[b];
        int buf = field[pa.first][pa.second];
        Cell(pa.first, pa.second) = field[pb.first][pb.second];
        Cell(pb.first, pb.second) = buf;
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
        int no, inds[9];
        RandomMutationIndices(no, inds);
        for (int k = 0; k < no; k++)
        {
            int i = inds[k];
            for (int j = 0; j < 9; j++)
                Cell(j, i) = other.field[j][i];
        }
    }
    void RowCrossover(const Chromosome& other)
    {
        int no, inds[9];
        RandomMutationIndices(no, inds);
        for (int k = 0; k < no; k++)
        {
            int i = inds[k];
            for (int j = 0; j < 9; j++)
                Cell(i, j) = other.field[i][j];
        }
    }
    void BlockCrossover(const Chromosome& other)
    {
        int no, inds[9];
        RandomMutationIndices(no, inds);
        for (int k = 0; k < no; k++)
        {
            int blk = inds[k];
            int xstart, xend, ystart, yend;
            Sudoku::Block(blk, xstart, xend, ystart, yend);
            for (int i = ystart; i < yend; i++)
                for (int j = xstart; j < xend; j++)
                    Cell(i, j) = other.field[i][j];
        }
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
