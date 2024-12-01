#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Usage: givens testFilename.txt" << endl;
        return 1;
    }
    ifstream fs(argv[1]);
    int givens = 0;
    for (int i = 0; i < 81; i++)
    {
        char ch; fs >> ch;
        givens += ch >= '1' && ch <= '9';
    }
    cout << givens << endl;
    return 0;
}
