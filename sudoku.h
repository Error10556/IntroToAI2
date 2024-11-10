#pragma once
#include <bitset>
#include <vector>

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
    static void Block(int no, int& xstart, int& xend, int& ystart, int& yend);

public:
    static int BlockNo(int row, int col);
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
