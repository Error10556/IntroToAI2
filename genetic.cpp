#include <algorithm>
#include <array>
#include <bitset>
#include <cstring>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <vector>
#include <ctime>
using namespace std;

// A data structure that efficiently maintains the counts of digits in a single
// row, column, or block
class DigitCounter
{
    // Each byte contains 2 counters, 4 bits for each.
    unsigned char counts[5]{};
    // Excess counts = how many digits we must remove to get rid of duplicates
    // Error pairs = how many pairs of the same digit we could pick
    // Zeros are ignored
    unsigned char excessCounts = 0, errorPairs = 0;
    // how many digits we must remove to get rid of duplicates
    static inline unsigned char Excess(unsigned char cnt)
    {
        return cnt - !!cnt;
    }
    // how many pairs of the same digit we could pick
    static inline unsigned char ErrorPairs(unsigned char cnt)
    {
        return (cnt * (cnt - 1)) >> 1;
    }
    // The byte where the count of digit 'num' is stored
    inline unsigned char& Dest(int num)
    {
        return counts[num >> 1];
    }
    // Get the lower 4 bits
    static inline unsigned char GetEven(unsigned char src)
    {
        return src & 0xF;
    }
    // Get the higher 4 bits
    static inline unsigned char GetOdd(unsigned char src)
    {
        return src >> 4;
    }
    // Set the lower 4 bits
    static inline void SetEven(unsigned char& dest, unsigned char cnt)
    {
        dest = 0xF0 & dest | (cnt & 0xF);
    }
    // Set the higher 4 bits
    static inline void SetOdd(unsigned char& dest, unsigned char cnt)
    {
        dest = 0xF & dest | ((cnt & 0xF) << 4);
    }

public:
    // Set the count of digit 'num'
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
    // Get the count of digit 'num'
    inline unsigned char Get(int num) const
    {
        unsigned char src = counts[num >> 1];
        if (num & 1)
            return GetOdd(src);
        return GetEven(src);
    }
    // Get the bitset where bitset[i] is 1 iff digit 'i' is present
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
    // Add d to the count of digit 'num'
    inline void Add(int num, unsigned char d)
    {
        Set(num, Get(num) + d);
    }
    // Increase the counter of digit 'num' by 1
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
    // Decrease the counter of digit 'num' by 1
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
    // Error pairs = how many pairs of the same digit we could pick
    // Zeros are ignored
    inline int ErrorPairs() const
    {
        return errorPairs;
    }
    // Excess counts = how many digits we must remove to get rid of duplicates
    // Zeros are ignored
    inline int ExcessCounts() const
    {
        return excessCounts;
    }
};

// Represents an integer modulo 1'000'000'007 (which is a prime)
// Useful for calculating polynomial hashes (see Sudoku::Hash)
class HashValue
{
public:
    // The modulo
    static constexpr int mod = (int)1e9 + 7;
    // Add 2 remainders
    static constexpr inline int modadd(int a, int b)
    {
        return (a + b) % mod;
    }
    // Subtract 2 remainders
    static constexpr inline int modsub(int a, int b)
    {
        return (a - b + mod) % mod;
    }
    // Multiply 2 remainders
    static constexpr inline int modmul(int a, int b)
    {
        return (long long)a * b % mod;
    }

private:
    // The remainder
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

// A helper class that calculates powers of Coef modulo 1'000'000'007
// at compile time! (see HashValue)
// Useful for calculating polynomial hashes (see Sudoku::Hash)
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
    // Powers[i] == i**10 % (HashValue::mod)
    static constexpr std::array<HashValue, Size> Powers = GenerateModPowers();
};

// The 9x9 field of digits with additional attributes:
// 1. each digit could be "initial", i.e. given in the test;
// 2. all instances of digits in each row, column, and block are counted.
// For the chromosome representation, see Chromosome.
// Each digit is in the range [0, 9], where '0' means 'empty'.
// The blocks are numbered as follows:
// 0 1 2
// 3 4 5
// 6 7 8
class Sudoku
{
public:
    // A structure that allows to assign numbers to cells.
    struct DigitReference
    {
    private:
        Sudoku& owner;
        int index;
        // Returns the reference to the variable where the digit is stored
        inline unsigned char& Ref();

    public:
        // Index is (row * 9 + column)
        DigitReference(Sudoku& owner, int index);
        // Reassign the digit
        DigitReference& operator=(int val);
        // Convert to a simple integer
        operator int();
    };

private:
    static constexpr int hashcoef = 10;
    // Powers of 10 modulo 1'000'000'007 (up to 10^80)
    using Powers = ModPowers<hashcoef, 81>;

    // The field is initialized with all empty cells (zeros).
    unsigned char field[9][9]{};
    // Use bitsets for compactness and efficiency
    bitset<9> initial[9];
    // Counters of digits in each row, column, and block.
    DigitCounter crows[9], ccols[9], cblocks[9];
    // See DigitCounter::ExcessCounts and DigitCounter::ErrorPairs
    int excessCounts = 0, errorPairs = 0;
    // The total number of nonempty cells
    int nonzeros = 0;
    // The hash (see Sudoku::Hash())
    HashValue hashval;

public:
    // The index of block where (row, col) is
    static constexpr int BlockNo(int row, int col);
    // The ranges of rows and columns of the block 'no'
    static void Block(int no, int& xstart, int& xend, int& ystart, int& yend);
    // Empty field
    Sudoku();
    // Assign a field from a list of rows. All non-digits are treated as '0'
    // ('empty')
    Sudoku(const std::vector<std::vector<int>>& field);
    std::vector<int> Row(int row) const;
    std::vector<int> Column(int col) const;
    // The numbers in the block are returned row-by-row, from left to right
    std::vector<int> Block(int no) const;
    // Gets or sets (use operator=) the flag that shows if the digit is given
    // in the statement
    bitset<9>::reference Initial(int r, int c);
    // Gets the flag that shows if the digit is given in the statement
    bool Initial(int r, int c) const;
    // Gets or sets (use operator=) the digit at row r, column c.
    // All non-digits are treated as '0' ('empty')
    DigitReference Cell(int r, int c);
    // Gets the digit at row r, column c.
    int Cell(int r, int c) const;
    // A convenient way to use a Sudoku object as a 2-dim array
    const unsigned char* operator[](int row) const;
    // Returns a bitset where bitset[i] == 1 iff 'i' is not present in the same
    // row, column, or block as the cell (row, col) (including itself)
    std::bitset<10> Available(int row, int col) const;
    // Marks all nonempty cells as initial and all empty cells as not initial.
    void FreezeAll();
    // Error pairs = how many pairs of the same digit we could pick
    // Zeros are ignored
    inline int ErrorPairCount() const;
    // Excess counts = how many digits we must remove to get rid of duplicates
    // Zeros are ignored
    inline int ExcessCounts() const;
    // The total number of nonempty cells
    inline int NonzeroCount() const;
    // The polynomial hash of the field's cells as strings of characters (row by
    // row, from left to right).
    // Calculated as (f[0]*10^0 + f[1]*10^1 + ... + f[i]*10^i + ...
    // ... + f[80]*10^80) mod (HashValue::mod),
    // where f[n] is field[n / 9][n % 9]
    inline HashValue Hash() const
    {
        return hashval;
    }
    // Returns if the sudoku comes earlier than (-1), later than (1), or is
    // equal to 'b' in a certain ordering.
    int Compare(const Sudoku& b) const;
    // Returns true if the sudoku comes earlier than 'b' in a certain ordering.
    bool operator<(const Sudoku& b) const;
    // Returns true if the sudoku is equal to 'b'.
    bool operator==(const Sudoku& b) const;
    bool operator!=(const Sudoku& b) const;
};

Sudoku::DigitReference::DigitReference(Sudoku& owner, int index)
    : owner(owner), index(index)
{
}

unsigned char& Sudoku::DigitReference::Ref()
{
    return owner.field[index / 9][index % 9];
}

Sudoku::DigitReference& Sudoku::DigitReference::operator=(int other)
{
    if (other < 0 || other > 9)
        other = 0;
    auto& ref = Ref();
    // Don't waste time if not necessary
    if (ref == other)
        return *this;
    // BEGIN remove the old digit from statistics
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
    // END remove the old digit from statistics
    // Update the hash
    owner.hashval += Sudoku::Powers::Powers[index] * (other - ref);
    ref = other;
    // BEGIN add the new digit to statistics
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
    // END add the new digit to statistics
    return *this;
}

Sudoku::DigitReference::operator int()
{
    return Ref();
}

// All zeros in all rows, columns, and blocks
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
            auto& dest = field[i][j];
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

const unsigned char* Sudoku::operator[](int row) const
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
    // Zero is never returned as 'available', so we remove the lowest bit
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

// Outputs the field
ostream& operator<<(ostream& out, const Sudoku& s)
{
    for (int i = 0; i < 9; i++)
    {
        const unsigned char* row = s[i];
        for (int j = 0; j < 9; j++)
        {
            if (j)
                out << ' ';
            if (row[j])
                out << (int)row[j];
            else
                out << '-';
        }
        out << '\n';
    }
    return out;
}

// Reads the field from a stream, discards the previous contents
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

// Returns the index of a random bit that is set to 1
template <int Size> int ChooseBit(bitset<Size> avail)
{
    int n = avail.count();
    n = rand() % n;
    size_t cur = avail._Find_first();
    for (; n; --n)
        cur = avail._Find_next(cur);
    return cur;
}

// The representation of the chromosome for the Evolutionary algorithm
// Contains a Sudoku field, but provides new methods for mutation and crossover
class Chromosome
{
private:
    Sudoku field;
    // A Coordinate filter is a predicate of a cell
    typedef bool (*CoordFilter)(const Sudoku& field, int y, int x);
    // Returns a vector of all coords that satisfy the filter
    vector<pair<int, int>> AllCoords(CoordFilter filter)
    {
        vector<pair<int, int>> res;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (filter(field, i, j))
                    res.emplace_back(i, j);
        return res;
    }
    // Returns all coordinates that we are allowed to change
    vector<pair<int, int>> AllNonInitialCoords()
    {
        return AllCoords([](const Sudoku& field, int y, int x) {
            return !field.Initial(y, x);
        });
    }
    // Returns all empty cells
    vector<pair<int, int>> AllEmptyCoords()
    {
        return AllCoords(
            [](const Sudoku& field, int y, int x) { return !field[y][x]; });
    }
    // Returns all cells that have been set by the algorithm
    vector<pair<int, int>> AllNonInitialFilledCoords()
    {
        return AllCoords([](const Sudoku& field, int y, int x) {
            return !field.Initial(y, x) && field[y][x];
        });
    }

    // Used in crossovers.
    // Outputs a shuffled array of numbers from 0 to 8 into the inds array;
    // only randint[1, 8] integers are shuffled ('no').
    static void RandomMutationIndices(int& no, int inds[9])
    {
        no = rand() % 8 + 1;
        iota(inds, inds + 9, 0);
        for (int i = 0; i < no; i++)
            swap(inds[i], inds[i + rand() % (9 - i)]);
    }

public:
    // Store the field.
    Chromosome(const Sudoku& f) : field(f)
    {
    }
    const Sudoku& Field() const
    {
        return field;
    }
    // The theoretically best fitness
    static constexpr int MaxFitness = 81;
    // The theoretically worst fitness (see Fitness())
    static constexpr int MinFitness = 81 - 27 * (9 * 8) / 2;
    // The empty sudoku has fitness 0
    // fitness = (# of filled cells) - (# of distinct unordered pairs of cells
    // that are in a conflict)
    int Fitness() const
    {
        int badpairs = field.ErrorPairCount();
        return field.NonzeroCount() - badpairs;
    }
    // Normalizes the fitness into the range [0, 1]
    float NormFitness() const
    {
        auto res = (Fitness() - MinFitness) / (float)(MaxFitness - MinFitness);
        // Square the result to bring the empty sudoku's fitness down
        return res * res;
    }
    // Replace an empty cell with a digit
    bool MutateGrow()
    {
        auto empty = AllEmptyCoords();
        if (empty.empty())
            return false;
        pair<int, int> p = empty[rand() % empty.size()];
        field.Cell(p.first, p.second) = rand() % 9 + 1;
        return true;
    }
    // Erase a digit in a non-initial cell
    bool MutateRemove()
    {
        auto nonempty = AllNonInitialFilledCoords();
        if (nonempty.empty())
            return false;
        auto p = nonempty[rand() % nonempty.size()];
        field.Cell(p.first, p.second) = 0;
        return true;
    }
    // Replace a digit in a non-initial cell with a different one
    bool MutateChange()
    {
        auto nonempty = AllNonInitialFilledCoords();
        if (nonempty.empty())
            return false;
        auto p = nonempty[rand() % nonempty.size()];
        auto cell = field.Cell(p.first, p.second);
        int prev = cell;
        // Choose a different digit
        int nw = rand() % 8 + 1;
        nw += nw >= prev;
        cell = nw;
        return true;
    }
    // Swap two digits in the same region (row, column, or block)
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
    // Swap two digits inside a row
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
    // Swap two digits inside a column
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
    // Swap two digits inside a block
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
    // Swap two numbers in a block
    bool MutateSwap()
    {
        return MutateSwapInBlock();
        // The code below tries to swap digits in other kinds of regions.
        // It has been left unused to leave the blocks consistent.
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
    // Randomly mutate
    void Mutate()
    {
        // Do not remove
        int modes[]{0, /*1, */ 2, 3}; // grow, remove, change, swap
        for (int i = 0; i < sizeof(modes) / sizeof(modes[0]); i++)
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
    // Replace some of the columns with ones from 'other'
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
    // Replace some of the rows with ones from 'other'
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
    // Replace some of the blocks with ones from 'other'
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
    // Replace some of the blocks with ones from 'other'
    void Crossover(const Chromosome& other)
    {
        BlockCrossover(other);
        return;
        // The code below may crossover via columns or rows.
        // It has been left unused to keep the blocks valid.
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

// A segment tree that samples several *distinct* elements according to weights
class FitSampler
{
    // The node of the binary tree
    struct Node
    {
        // The segment is [l, r)
        int l, r;
        // The sum of weights on the segment
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
    // The binary tree is indexed s.t. node i has children i*2+1 and i*2+2
    vector<Node> st;
    // The first index of the row of leaves
    int start;

public:
    // Fitnesses are weights. They make the leaves of the tree.
    // The nodes above store the sum of their children's weights.
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
    // Change the weight of leaf 'ind' to 'val' in O(log n)
    void SetWeight(int ind, int val)
    {
        ind += start;
        // We are going to be adding the new value of 'val' to the nodes
        val -= st[ind].sum;
        st[ind].sum += val;
        // Ascent to the root updates the sums on the containing segments
        while (ind != 0)
        {
            ind = (ind - 1) / 2;
            st[ind].sum += val;
        }
    }
    // Returns a randomly chosen index and sets its weight to 0
    int Sample()
    {
        // If nothing is left
        if (st[0].sum == 0)
            return -1;
        int ind = rand() % st[0].sum;
        // We start the segment tree descent
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
        // Convert from tree indices to normal indices
        cur -= start;
        // 'Remove' the leaf
        SetWeight(cur, 0);
        return cur;
    }
};

// Orders the chromosomes in a set in an order of decreasing fitness;
// removes duplicates
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

// This data structure contains a set of Chromosomes and performs mutations,
// crossovers, and 'natural selection'
class Population
{
    // The quota: maximum number of chromosomes
    int n;
    // The population of chromosomes
    set<Chromosome, FitnessComparator> pop;

    // Returns true with a chance of 'chance' out of 1
    static bool TestChance(float chance)
    {
        return rand() / (double)RAND_MAX < chance;
    }

public:
    // Initialize the population: spawn 'mutants' mutants, each undergoes
    // 'mutations' mutations
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
        // Remove some if there are too many
        KillExcess(1);
    }

    // Mutate at least 'minMutationCount' chromosomes, but keep going until we
    // reach the quota. Each chromosome mutates [1, mutationMax] times.
    // Choose more fit chromosomes in hopes for quick advancement.
    void Mutate(int minMutationCount, int mutationMax)
    {
        int sz = pop.size();
        vector<int> fits(sz);
        vector<decltype(pop)::iterator> iters(sz);
        auto iter = pop.begin();
        const int FitnessRange =
            Chromosome::MaxFitness - Chromosome::MinFitness;
        for (int i = 0; i < sz; i++, ++iter)
        {
            int& curfit = fits[i];
            // Punish lower fitnesses by squaring
            curfit = iter->Fitness() - Chromosome::MinFitness;
            curfit *= curfit;
            curfit /= FitnessRange;
            iters[i] = iter;
        }
        // Build a cumulative sum array
        for (int i = 1; i < sz; i++)
            fits[i] += fits[i - 1];
        while (minMutationCount > 0 || pop.size() < n)
        {
            // Pick a chromosome to mutate
            int rnd = rand() % fits.back();
            int ind = upper_bound(fits.begin(), fits.end(), rnd) - fits.begin();
            auto iter = iters[rand() % sz];
            // The mutant
            Chromosome mut = *iter;
            for (int i = rand() % mutationMax + 1; i; --i)
                mut.Mutate();
            pop.insert(mut);
            minMutationCount -= !!minMutationCount;
        }
    }

    // Perform 'noLuckyChromosomes' crossovers, where one parent is sampled
    // according to the fitness, and another is picked at random.
    void Crossover(int noLuckyChromosomes, int childrenPerCouple)
    {
        int sz = pop.size();
        if (sz < 2)
            return;
        // For a chromosome i, fits[i] is its fitness, and iters[i] is its
        // iterator in the set.
        vector<int> fits(sz);
        vector<set<Chromosome>::iterator> iters(sz);
        {
            auto iter = pop.begin();
            for (int i = 0; i < sz; i++, iter++)
            {
                // Each one gets bonus points (+(max - min) / 5)
                fits[i] = iter->Fitness() - Chromosome::MinFitness +
                          (Chromosome::MaxFitness - Chromosome::MinFitness) / 5;
                iters[i] = iter;
            }
        }

        FitSampler samp(fits);
        for (int i = 0; i < noLuckyChromosomes; i++)
        {
            // Pick a mother
            int mother = samp.Sample();
            if (mother == -1)
                continue;
            // Pick a father randomly, but it must be a different chromosome
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

    // Remove chromosomes until there are not more than 'n' left.
    // The most fit are immune, but may be removed if all other chromosomes
    // have already been erased.
    // The 'elites' best chromosomes are also immune.
    void KillExcess(int elites)
    {
        int sz = pop.size();
        if (sz <= n || elites >= sz)
            return;
        vector<int> fits(sz);
        // The maximum fitness; the chromosomes with fitness 'mxFit' are immune
        int mxFit = pop.begin()->Fitness();
        {
            auto iter = pop.begin();
            advance(iter, elites);
            // Chromosomes with low fitnesses are likely to be picked.
            // The best chromosomes are given weight 0.
            for (int i = elites; i < sz; i++, ++iter)
                fits[i] = mxFit - iter->Fitness();
        }
        FitSampler samp(fits);
        // We mark a chromosome i for death by setting excess[i] = true
        vector<bool> excess(sz);
        for (int i = sz - n; i; --i)
        {
            int ind = samp.Sample();
            if (ind != -1)
                excess[ind] = true;
        }
        auto iter = pop.begin();
        advance(iter, elites);
        // Erase all excessive chromosomes
        for (int i = elites; i < sz; i++)
        {
            if (excess[i])
                iter = pop.erase(iter);
            else
                ++iter;
        }
        // If still too many, remove from the tail
        if (pop.size() > n)
        {
            auto iter = pop.end();
            advance(iter, n - pop.size());
            pop.erase(iter, pop.end());
        }
    }

    // Do a cycle of evolution
    void EvolutionStep(int noLuckyChromosomes, int childrenPerCouple,
                       int elites, int minMutationCount, int mutationMax)
    {
        Crossover(noLuckyChromosomes, childrenPerCouple);
        Mutate(minMutationCount, mutationMax);
        KillExcess(elites);
    }

    // See all chromosomes
    const set<Chromosome, FitnessComparator>& AllChromosomes() const
    {
        return pop;
    }

    // See the best chromosome
    const Chromosome& Best() const
    {
        return *pop.begin();
    }
};

int main(int argc, char** argv)
{
    // For debugging
    bool verbose = false;
    for (int i = 1; i < argc; i++)
    {
        char* str = argv[i];
        if (strcmp(str, "--verbose") == 0)
            verbose = true;
    }
    // Initialize rand()
    srand(clock());
    Sudoku sd;
    cin >> sd;
    // If for 'MaxPatience' iterations we won't see any improvements, we retry
    const int PopulationMax = 500, MaxPatience = 10000;
    int patience = MaxPatience;
    // Initialize the population
    Population pop(PopulationMax, sd, PopulationMax / 3, 20);
    if (verbose)
        cout << "Initial: " << pop.Best().Fitness() << endl;
    Chromosome prevbest = Chromosome(sd);
    // prevnbest = the number of chromosomes with the best fitness
    // For debugging purposes
    int prevnbest = 0;
    int prevfit = prevbest.Fitness();
    int curfit = prevfit;
    // Repeat until we find the solution
    while (curfit != Chromosome::MaxFitness)
    {
        pop.EvolutionStep(PopulationMax / 10, 5, 3, PopulationMax * 2, 3);
        curfit = pop.Best().Fitness();
        if (curfit == prevfit)
            patience--;
        else
            patience = MaxPatience;
        if (patience == 0)
        {
            // Retry
            if (verbose)
                cout << "Restarting..." << endl;
            // Initialize everything again
            pop = Population(PopulationMax, sd, PopulationMax / 3, 20);
            prevbest = pop.Best();
            curfit = prevfit = prevbest.Fitness();
            patience = MaxPatience;
            continue;
        }
        prevfit = curfit;
        if (!verbose)
            continue;
        // "Verbose" section: output improvements
        auto best = pop.Best();
        int noBest = 0;
        auto all = pop.AllChromosomes();
        auto iter = all.begin();
        for (; iter != all.end() && iter->Fitness() == best.Fitness();
             ++iter, noBest++)
            ;
        if (noBest != prevnbest || prevbest.Field() != best.Field())
        {
            // The best sudoku so far
            cout << "\n\n" << pop.Best().Field();
            // Fitness out of 81 (The number of chromosome with best fitness)
            cout << best.Fitness() << '/' << Chromosome::MaxFitness << " ("
                 << noBest << ")\n";
            cout.flush();
            prevbest = best;
            prevnbest = noBest;
        }
    }
    // If not in verbose, we have not seen the solution
    if (!verbose)
        cout << pop.Best().Field();
}
