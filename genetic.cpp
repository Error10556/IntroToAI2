#include <algorithm>
#include <array>
#include <bitset>
#include <cstring>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <vector>
using namespace std;

class DigitCounter
{
    unsigned char counts[5]{};
    unsigned char excessCounts = 0, errorPairs = 0;
    static inline unsigned char Excess(unsigned char cnt)
    {
        return cnt - !!cnt;
    }
    static inline unsigned char ErrorPairs(unsigned char cnt)
    {
        return (cnt * (cnt - 1)) >> 1;
    }
    inline unsigned char& Dest(int num)
    {
        return counts[num >> 1];
    }
    static inline unsigned char GetEven(unsigned char src)
    {
        return src & 0xF;
    }
    static inline unsigned char GetOdd(unsigned char src)
    {
        return src >> 4;
    }
    static inline void SetEven(unsigned char& dest, unsigned char cnt)
    {
        dest = 0xF0 & dest | (cnt & 0xF);
    }
    static inline void SetOdd(unsigned char& dest, unsigned char cnt)
    {
        dest = 0xF & dest | ((cnt & 0xF) << 4);
    }

public:
    inline void Set(int num, unsigned char cnt)
    {
        unsigned char& dest = Dest(num);
        if (num & 1)
        {
            excessCounts -= Excess(GetOdd(dest));
            errorPairs -= ErrorPairs(GetOdd(dest));
            SetOdd(dest, cnt);
        }
        else
        {
            if (num)
            {
                excessCounts -= Excess(GetEven(dest));
                errorPairs -= ErrorPairs(GetEven(dest));
            }
            SetEven(dest, cnt);
        }
        if (num)
        {
            excessCounts += Excess(cnt);
            errorPairs += ErrorPairs(cnt);
        }
    }
    inline unsigned char Get(int num) const
    {
        unsigned char src = counts[num >> 1];
        if (num & 1)
            return GetOdd(src);
        return GetEven(src);
    }
    inline bitset<10> NonzeroMap() const
    {
        int res = 0;
        for (int i = 4; i >= 0; i--)
        {
            unsigned char cnt = counts[i];
            res = (res << 2) | ((!!(cnt & 0xF0)) << 1) | (!!(cnt & 0xF));
        }
        return res;
    }
    inline void Add(int num, unsigned char d)
    {
        Set(num, Get(num) + d);
    }
    inline void Inc(int num)
    {
        unsigned char& dest = Dest(num);
        if (num & 1)
        {
            excessCounts += !!(dest & 0xF0);
            errorPairs += dest >> 4;
            dest += 0x10;
        }
        else
        {
            if (num)
            {
                excessCounts += !!(dest & 0xF);
                errorPairs += dest & 0xF;
            }
            dest++;
            dest -= 0x10 * !(dest & 0xF);
        }
    }
    inline void Dec(int num)
    {
        unsigned char& dest = Dest(num);
        if (num & 1)
        {
            dest -= 0x10;
            excessCounts -= !!(dest & 0xF0);
            errorPairs -= dest >> 4;
        }
        else
        {
            dest += 0x10 * !(dest & 0xF);
            dest--;
            if (num)
            {
                excessCounts -= !!(dest & 0xF);
                errorPairs -= dest & 0xF;
            }
        }
    }
    inline int ErrorPairs() const
    {
        return errorPairs;
    }
    inline int ExcessCounts() const
    {
        return excessCounts;
    }
};

class HashValue
{
public:
    static constexpr int mod = (int)1e9 + 7;
    static constexpr inline int modadd(int a, int b)
    {
        return (a + b) % mod;
    }
    static constexpr inline int modsub(int a, int b)
    {
        return (a - b + mod) % mod;
    }
    static constexpr inline int modmul(int a, int b)
    {
        return (long long)a * b % mod;
    }

private:
    int val = 0;

public:
    constexpr HashValue(int val) : val((val % mod + mod) % mod)
    {
    }
    constexpr HashValue()
    {
    }
    constexpr HashValue& operator=(int val)
    {
        return *this = HashValue(val);
    }
    constexpr HashValue& operator+=(const HashValue& other)
    {
        val = modadd(val, other.val);
        return *this;
    }
    constexpr HashValue& operator-=(const HashValue& other)
    {
        val = modsub(val, other.val);
        return *this;
    }
    constexpr HashValue& operator*=(const HashValue& other)
    {
        val = modmul(val, other.val);
        return *this;
    }
    constexpr HashValue operator-() const
    {
        return {mod - val};
    }
    constexpr bool operator==(const HashValue& other) const
    {
        return val == other.val;
    }
    constexpr bool operator!=(const HashValue& other) const
    {
        return val != other.val;
    }
    constexpr bool operator<(const HashValue& other) const
    {
        return val < other.val;
    }
    constexpr bool operator<=(const HashValue& other) const
    {
        return val <= other.val;
    }
    constexpr HashValue operator+(const HashValue& other) const
    {
        return {modadd(val, other.val)};
    }
    constexpr HashValue operator-(const HashValue& other) const
    {
        return {modsub(val, other.val)};
    }
    constexpr HashValue operator*(const HashValue& other) const
    {
        return {modmul(val, other.val)};
    }
    constexpr explicit operator int() const
    {
        return val;
    }
};

template <int Coef, int Size> class ModPowers
{
private:
    static constexpr std::array<HashValue, Size> GenerateModPowers()
    {
        std::array<HashValue, Size> powers;
        powers[0] = 1;
        for (int i = 1; i < Size; i++)
            powers[i] = powers[i - 1] * Coef;
        return powers;
    }

public:
    static constexpr std::array<HashValue, Size> Powers = GenerateModPowers();
};

class Sudoku
{
public:
    struct DigitReference
    {
    private:
        Sudoku& owner;
        int index;
        inline int& Ref();

    public:
        DigitReference(Sudoku& owner, int index);
        DigitReference& operator=(int val);
        operator int();
    };

private:
    static constexpr int hashcoef = 10;
    using Powers = ModPowers<hashcoef, 81>;

    int field[9][9]{};
    bitset<9> initial[9];
    DigitCounter crows[9], ccols[9], cblocks[9];
    int excessCounts = 0, errorPairs = 0;
    int nonzeros = 0;
    HashValue hashval;

public:
    static constexpr int BlockNo(int row, int col);
    static void Block(int no, int& xstart, int& xend, int& ystart, int& yend);
    Sudoku();
    Sudoku(const std::vector<std::vector<int>>& field);
    std::vector<int> Row(int row) const;
    std::vector<int> Column(int col) const;
    std::vector<int> Block(int no) const;
    bitset<9>::reference Initial(int r, int c);
    bool Initial(int r, int c) const;
    DigitReference Cell(int r, int c);
    int Cell(int r, int c) const;
    const int* operator[](int row) const;
    std::bitset<10> Available(int row, int col) const;
    void FreezeAll();
    int ErrorPairCount() const;
    int ExcessCounts() const;
    int NonzeroCount() const;
    inline HashValue Hash() const
    {
        return hashval;
    }
    int Compare(const Sudoku& b) const;
    bool operator<(const Sudoku& b) const;
    bool operator==(const Sudoku& b) const;
    bool operator!=(const Sudoku& b) const;
};

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

template <int Size> int ChooseBit(bitset<Size> avail)
{
    int n = avail.count();
    n = rand() % n;
    size_t cur = avail._Find_first();
    for (; n; --n)
        cur = avail._Find_next(cur);
    return cur;
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

    static void RandomMutationIndices(int& no, int inds[9])
    {
        no = rand() % 8 + 1;
        iota(inds, inds + 9, 0);
        for (int i = 0; i < no; i++)
            swap(inds[i], inds[i + rand() % (9 - i)]);
    }

public:
    Chromosome(const Sudoku& f) : field(f)
    {
    }
    const Sudoku& Field() const
    {
        return field;
    }
    static constexpr int MaxFitness = 81;
    static constexpr int MinFitness = 81 - 27 * (9 * 8) / 2;
    // The empty sudoku has fitness 0
    int Fitness() const
    {
        int badpairs = field.ErrorPairCount();
        return field.NonzeroCount() - (!!badpairs) * 5 - badpairs;
    }
    float NormFitness() const
    {
        auto res = (Fitness() - MinFitness) / (float)(MaxFitness - MinFitness);
        return res * res;
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
        field.Cell(p.first, p.second) = ChooseBit<10>(avail);
        return true;
    }
    bool MutateRemove()
    {
        auto nonempty = AllNonInitialFilledCoords();
        if (nonempty.empty())
            return false;
        auto p = nonempty[rand() % nonempty.size()];
        field.Cell(p.first, p.second) = 0;
        return true;
    }
    bool MutateChange()
    {
        auto nonempty = AllNonInitialFilledCoords();
        while (nonempty.size())
        {
            int ind = rand() % nonempty.size();
            auto p = nonempty[ind];
            auto cell = field.Cell(p.first, p.second);
            auto avail = field.Available(p.first, p.second);
            if (avail.none())
            {
                nonempty[ind] = nonempty.back();
                nonempty.pop_back();
                continue;
            }
            cell = ChooseBit<10>(avail);
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
        field.Cell(pa.first, pa.second) = field[pb.first][pb.second];
        field.Cell(pb.first, pb.second) = buf;
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
        return MutateSwapInBlock();
        vector<int> modes{0, 1, 2};
        swap(modes[0], modes[rand() % 3]);
        swap(modes[1], modes[rand() % 2 + 1]);
        for (int mode : modes)
            switch (mode)
            {
            case 0:
                if (MutateSwapInRow())
                    return true;
                break;
            case 1:
                if (MutateSwapInColumn())
                    return true;
                break;
            case 2:
                if (MutateSwapInBlock())
                    return true;
                break;
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
                break;
            case 1:
                if (MutateRemove())
                    return;
                break;
            case 2:
                if (MutateChange())
                    return;
                break;
            case 3:
                if (MutateSwap())
                    return;
                break;
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
                field.Cell(j, i) = other.field[j][i];
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
                field.Cell(i, j) = other.field[i][j];
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
                    field.Cell(i, j) = other.field[i][j];
        }
    }
    void Crossover(const Chromosome& other)
    {
        BlockCrossover(other);
        return;
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

class FitSampler
{
    struct Node
    {
        int l, r;
        int sum;
        void Init(int i, int val)
        {
            l = i;
            r = i + 1;
            sum = val;
        }
        void Recalc(Node& left, Node& right)
        {
            sum = left.sum + right.sum;
        }
        void Merge(Node& left, Node& right)
        {
            l = left.l;
            r = right.r;
            Recalc(left, right);
        }
    };
    vector<Node> st;
    int start;

public:
    FitSampler(const vector<int>& fitnesses)
    {
        int n = fitnesses.size();
        int sz = n & -n;
        while (sz < n)
            sz <<= 1;
        st.resize(sz * 2 - 1);
        start = sz - 1;
        for (int i = 0; i < sz; i++)
        {
            Node& nd = st[start + i];
            nd.Init(i, i < n ? fitnesses[i] : 0);
        }
        for (int i = start - 1; i >= 0; i--)
            st[i].Merge(st[i * 2 + 1], st[i * 2 + 2]);
    }
    void SetWeight(int ind, int val)
    {
        ind += start;
        val -= st[ind].sum;
        st[ind].sum += val;
        while (ind != 0)
        {
            ind = (ind - 1) / 2;
            st[ind].sum += val;
        }
    }
    int Sample()
    {
        if (st[0].sum == 0)
            return -1;
        int ind = rand() % st[0].sum;
        int cur = 0;
        while (cur < start)
        {
            cur = cur * 2 + 1;
            if (ind >= st[cur].sum)
            {
                ind -= st[cur].sum;
                cur++;
            }
        }
        cur -= start;
        SetWeight(cur, 0);
        return cur;
    }
};

class FitnessComparator
{
public:
    bool operator()(const Chromosome& a, const Chromosome& b) const
    {
        int afit = a.Fitness();
        int bfit = b.Fitness();
        if (afit != bfit)
            return afit > bfit;
        return a.Field() < b.Field();
    }
};

class Population
{
    int n;
    set<Chromosome, FitnessComparator> pop;

    static bool TestChance(float chance)
    {
        return rand() / (double)RAND_MAX < chance;
    }

public:
    Population(int n, const Sudoku& init, int mutants, int mutations) : n(n)
    {
        auto ch = Chromosome(init);
        pop.insert(ch);
        while (mutants--)
        {
            auto nw = ch;
            for (int i = 0; i < mutations; i++)
                nw.Mutate();
            pop.insert(nw);
        }
        KillExcess(1);
    }

    void Mutate(int minMutationCount, int mutationMax)
    {
        int sz = pop.size();
        vector<decltype(pop)::iterator> iters(sz);
        auto iter = pop.begin();
        for (int i = 0; i < sz; i++, ++iter)
            iters[i] = iter;
        while (minMutationCount > 0 || pop.size() < n)
        {
            auto iter = iters[rand() % sz];
            Chromosome mut = *iter;
            for (int i = rand() % mutationMax + 1; i; --i)
                mut.Mutate();
            pop.insert(mut);
            minMutationCount -= !!minMutationCount;
        }
    }

    void Crossover(int noLuckyChromosomes, int childrenPerCouple)
    {
        int sz = pop.size();
        if (sz < 2)
            return;
        vector<int> fits(sz);
        vector<set<Chromosome>::iterator> iters(sz);
        {
            auto iter = pop.begin();
            for (int i = 0; i < sz; i++, iter++)
            {
                fits[i] = iter->Fitness() - Chromosome::MinFitness +
                          (Chromosome::MaxFitness - Chromosome::MinFitness) / 5;
                iters[i] = iter;
            }
        }

        FitSampler samp(fits);
        for (int i = 0; i < noLuckyChromosomes; i++)
        {
            int mother = samp.Sample();
            if (mother == -1)
                continue;
            int father = rand() % (sz - 1);
            father += father >= mother;
            for (int j = 0; j < childrenPerCouple; j++)
            {
                Chromosome child = *iters[mother];
                child.Crossover(*iters[father]);
                pop.insert(child);
            }
        }
    }

    void KillExcess(int elites)
    {
        int sz = pop.size();
        if (sz <= n || elites >= sz)
            return;
        vector<int> fits(sz);
        int mxFit = pop.begin()->Fitness();
        {
            auto iter = pop.begin();
            advance(iter, elites);
            for (int i = elites; i < sz; i++, ++iter)
                fits[i] = mxFit - iter->Fitness();
        }
        FitSampler samp(fits);
        vector<bool> excess(sz);
        for (int i = sz - n; i; --i)
        {
            int ind = samp.Sample();
            if (ind != -1)
                excess[ind] = true;
        }
        auto iter = pop.begin();
        advance(iter, elites);
        for (int i = elites; i < sz; i++)
        {
            if (excess[i])
                iter = pop.erase(iter);
            else
                ++iter;
        }
        if (pop.size() > n)
        {
            auto iter = pop.begin();
            advance(iter, n);
            pop.erase(iter, pop.end());
        }
    }

    void EvolutionStep(int noLuckyChromosomes, int childrenPerCouple,
                       int elites, int minMutationCount, int mutationMax)
    {
        Crossover(noLuckyChromosomes, childrenPerCouple);
        Mutate(minMutationCount, mutationMax);
        KillExcess(elites);
    }

    const set<Chromosome, FitnessComparator>& AllChromosomes() const
    {
        return pop;
    }

    const Chromosome& Best() const
    {
        return *pop.begin();
    }
};

int main(int argc, char** argv)
{
    bool verbose = false;
    for (int i = 1; i < argc; i++)
    {
        char* str = argv[i];
        if (strcmp(str, "--verbose") == 0)
            verbose = true;
    }
    srand(clock());
    Sudoku sd;
    cin >> sd;
    const int PopulationMax = 300, MaxPatience = 4000;
    int patience = MaxPatience;
    Population pop(PopulationMax, sd, PopulationMax / 3, 20);
    if (verbose)
        cout << "Initial: " << pop.Best().Fitness() << endl;
    Chromosome prevbest = Chromosome(sd);
    int prevnbest = 0;
    int prevfit = prevbest.Fitness();
    int curfit = prevfit;
    while (curfit != Chromosome::MaxFitness)
    {
        pop.EvolutionStep(PopulationMax / 10, 5, 3, PopulationMax / 2, 3);
        curfit = pop.Best().Fitness();
        if (curfit == prevfit)
            patience--;
        else
            patience = MaxPatience;
        if (patience == 0)
        {
            if (verbose)
                cout << "Restarting..." << endl;
            pop = Population(PopulationMax, sd, PopulationMax / 3, 20);
            prevbest = pop.Best();
            curfit = prevfit = prevbest.Fitness();
            patience = MaxPatience;
        }
        prevfit = curfit;
        if (!verbose)
            continue;
        auto best = pop.Best();
        int noBest = 0;
        auto all = pop.AllChromosomes();
        auto iter = all.begin();
        for (; iter != all.end() && iter->Fitness() == best.Fitness();
             ++iter, noBest++)
            ;
        if (noBest != prevnbest || prevbest.Field() != best.Field())
        {
            cout << "\n\n" << pop.Best().Field();
            cout << best.Fitness() << '/' << Chromosome::MaxFitness << " ("
                 << noBest << ")\n";
            cout.flush();
            prevbest = best;
            prevnbest = noBest;
        }
    }
    if (!verbose)
        cout << pop.Best().Field();
}
