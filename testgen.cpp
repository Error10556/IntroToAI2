#include "sudokusolve.h"
#include <cstdio>
#include <iostream>
#include <climits>
using namespace std;

FILE* randgen;

int randint()
{
    int res;
    fread(&res, sizeof(int), 1, randgen);
    res &= INT_MAX;
    return res;
}

int randint(int n)
{
    return randint() % n;
}

int randint(int min, int exmax)
{
    return randint(exmax - min) + min;
}



int main(int argc, char** argv)
{
    randgen = fopen("/dev/random", "r");
    if (argc == 1)
    {
        cout << "Usage: testgen directory #OfInitialDigits1 #OfTests1 "
                "#OfInitDigits2 #OfTests2 ...\n";
        return 0;
    }
    fclose(randgen);
}
