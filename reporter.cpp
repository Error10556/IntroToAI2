#include "launcher.h"
#include <algorithm>
#include <bits/stdc++.h>
#include <cstdio>
using namespace std;

struct Stats
{
    int Max;
    double Avg;
};

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Usage: reporter solution.bin testdir" << endl;
        return 1;
    }
    string progFilename = argv[1];
    string testDir = argv[2];
    map<int, vector<Stats>> reports;
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
        int given = 81 - count(testcase.begin(), testcase.end(), '-');
        cout.flush();
        Process proc(progFilename, {progFilename, "--report"}, {});
        fputs(testcase.c_str(), proc.StdIN());
        fflush(proc.StdIN());
        proc.Wait();
        string output;
        {
            char buf[128];
            while (true)
            {
                char* res = fgets(buf, 128, proc.StdOUT());
                if (res == 0)
                    break;
                output.insert(output.end(), buf, buf + strlen(buf));
            }
        }
        const char* beginmark = "REPORT\n";
        int start = output.find(beginmark);
        start += strlen(beginmark);
        stringstream str(output.substr(start));
        Stats report;
        while (true)
        {
            string key;
            if (!(str >> key))
                break;
            if (key == "Max")
                str >> report.Max;
            else if (key == "Average")
                str >> report.Avg;
        }
        reports[given].push_back(report);
    }
    cout << "--Stats--\n";
    cout << "{\n";
    for (auto& p : reports)
    {
        cout << p.first << ": {";
        int smax = 0;
        double savg = 0;
        for (auto& s : p.second)
        {
            smax += s.Max;
            savg += s.Avg;
        }
        double n = p.second.size();
        cout << "\"Max\": " << smax / n << ", \"Avg\": " << savg / n << ", ";
        cout << "\"Avgs\": [";
        bool first = true;
        for (auto& s : p.second)
        {
            if (!first)
                cout << ", ";
            cout << s.Avg;
            first = false;
        }
        cout << "]},\n";
    }
    cout << "}" << endl;
}
