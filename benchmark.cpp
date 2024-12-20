#include "launcher.h"
#include <bits/stdc++.h>
#include <chrono>
#include <filesystem>
#include <numeric>
using namespace std;

template<class T>
ostream& operator<<(ostream& out, const vector<T>& v)
{
    out << "{ ";
    bool first = true;
    for (const auto& i : v)
    {
        if (!first)
            out << ", ";
        out << i;
        first = false;
    }
    return out << " }";
}

struct Stats
{
    double Max, Min, Mean, Median, SD;
    Stats(vector<double> v)
    {
        sort(v.begin(), v.end());
        Max = v.back();
        Min = v.front();
        int n = v.size();
        if (n & 1)
            Median = v[n >> 1];
        else
            Median = (v[n >> 1] + v[(n >> 1) - 1]) / 2;
        Mean = accumulate(v.begin(), v.end(), 0.0) / n;
        SD = 0;
        for (double i : v)
        {
            i -= Mean;
            SD += i * i;
        }
        SD = sqrt(SD / (n - 1));
    }
};

ostream& operator<<(ostream& out, const Stats& stats)
{
    out << "Range = [" << stats.Min << ", " << stats.Max << "]\n";
    out << "Median = " << stats.Median << '\n';
    out << "Mean = " << stats.Mean << '\n';
    return out << "SD = " << stats.SD << '\n';
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        cout << "Usage: benchmark solution.bin repeats testDir" << endl;
        return 1;
    }
    string progFilename = argv[1];
    int repeats = atoi(argv[2]);
    string testDir = argv[3];
    map<string, vector<double>> benchmarks;
    vector<filesystem::path> entries;
    for (auto& entry : filesystem::directory_iterator(testDir))
        entries.push_back(entry.path());
    sort(entries.begin(), entries.end());
    for (auto& entry : entries)
    {
        string testcase;
        {
            stringstream testin;
            testin << ifstream(entry, ios::in).rdbuf();
            testcase = testin.str();
        }
        string testFilename = entry.filename().string();
        cout << "Testing on " << testFilename << "..." << endl;
        vector<double>& curBenchmarks = benchmarks[testFilename];
        cout << '\r' << "0/" << repeats;
        for (int i = 1; i <= repeats; i++)
        {
            cout.flush();
            Process proc(progFilename, {}, {});
            fputs(testcase.c_str(), proc.StdIN());
            auto starttime = chrono::steady_clock::now();
            proc.Wait();
            auto endtime = chrono::steady_clock::now();
            auto dur = endtime - starttime;
            using frac = decltype(dur)::period;
            curBenchmarks.push_back(dur.count() * frac::num /
                                    (double)frac::den);
            cout << '\r' << i << '/' << repeats;
        }
        cout << endl;
    }
    cout << "--Stats--\n";
    for (auto& p : benchmarks)
    {
        cout << p.first << ": " << p.second << '\n';
        cout << Stats(p.second) << '\n';
    }
}
