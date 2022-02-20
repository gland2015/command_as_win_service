#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#define byte win_byte_override

using namespace std;
using namespace std::filesystem;

struct CmdArgs
{
    string cmd;
    string cwd;

    bool debug;

    string logFile;
    bool logIsAppend;
    bool logIsLogHead;
    long long logLimit;
};

struct LogOptions
{
    string filepath;
    long long limit;
    bool isLog;
    bool isAppend;
    bool isLogHead;
};

CmdArgs parseCmdArgs(int argc, char *argv[]);
string getCurExeDir();
string getCurExePath();
string getCurExeFileName();

