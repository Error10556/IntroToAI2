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
public:
    Sudoku();
    Sudoku(const std::vector<std::vector<int>>& field);
    const std::vector<int>& Row(int row) const;
    std::vector<int> Column(int col) const;
    std::vector<int> Block(int no) const;
    decltype(initial[0][0]) Initial(int r, int c);
    bool Initial(int r, int c) const;
    DigitReference Cell(int r, int c);
    int Cell(int r, int c) const;
};
