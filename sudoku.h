#pragma once
#include <bitset>
#include <istream>
#include <vector>
#include <array>
#include <iostream>

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
    inline std::bitset<10> NonzeroMap() const
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
    std::bitset<9> initial[9];
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
    std::bitset<9>::reference Initial(int r, int c);
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

std::ostream& operator<<(std::ostream& out, const Sudoku& sd);
std::istream& operator>>(std::istream& in, Sudoku& sd);
