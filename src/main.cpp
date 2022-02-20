#include "./base.h"
#include "stdio.h"
#include "tchar.h"
#include <atlstr.h>

#define NOMINMAX 1
#include "Windows.h"

#define SERVICE_NAME TEXT("")
SERVICE_STATUS ServiceStatus = {0};
SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL;

PROCESS_INFORMATION pi;
HANDLE hFile = NULL;

CmdArgs args;

extern bool execCmdline(PROCESS_INFORMATION &pi, HANDLE &hFile, const string &cmdline, bool waitComplete, const string &workdir, LogOptions &logOptions, unsigned int acp = 65001);

void runTask();
void WINAPI ServiceMain(int argc, char **argv);
void WINAPI ServiceHandler(DWORD fdwControl);

int main(int argc, char **argv)
{
    args = parseCmdArgs(argc, argv);

    if (args.debug)
    {
        string command = "";
        char **temp = argv;
        while (*temp != NULL)
        {
            string a = *temp;
            ++temp;
            command += a + " ";
        }

        path logPath = getCurExeDir();
        string filename = getCurExeFileName();
        filename += ".debug_info.txt";
        logPath /= filename;

        ofstream outfile;
        outfile.open(logPath);
        outfile << "command: " << command << endl;
        outfile << "cmd: " << args.cmd << endl;
        outfile << "cwd: " << args.cwd << endl;
        outfile << "logFile: " << args.logFile << endl;
        outfile << "logIsAppend: " << args.logIsAppend << endl;
        outfile << "logIsLogHead: " << args.logIsLogHead << endl;
        outfile << "logIsLogHead: " << args.logIsLogHead << endl;
        outfile << "logLimit: " << args.logLimit << endl;
        outfile.close();
    }

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".utf8");

    // 启动服务
    SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = SERVICE_NAME;
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}

void runTask()
{
    // 添加运行任务
    if (args.cwd.length() == 0)
    {
        args.cwd = getCurExeDir();
    }

    LogOptions logOpt;
    if (args.logFile.length())
    {
        logOpt.isLog = true;
        logOpt.filepath = args.logFile;
        logOpt.isAppend = args.logIsAppend;
        logOpt.isLogHead = args.logIsLogHead;
        logOpt.limit = args.logLimit;
    }
    else
    {
        logOpt.isLog = false;
    }

    execCmdline(pi, hFile, args.cmd, true, args.cwd, logOpt, GetACP());
}

// ---------------服务主函数----------------
void WINAPI ServiceMain(int argc, char **argv)
{
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceHandler);
    if (hServiceStatusHandle == 0)
    {
        DWORD nError = GetLastError();
    }

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;
    if (!SetServiceStatus(hServiceStatusHandle, &ServiceStatus))
    {
        DWORD nError = GetLastError();
    }

    runTask();

    ServiceStatus.dwControlsAccepted = 0;
    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus(hServiceStatusHandle, &ServiceStatus) == FALSE)
    {
    }
}

// -------------服务消息处理函数---------------
void WINAPI ServiceHandler(DWORD fdwControl)
{
    UINT uExitCode = NULL;
    DWORD dwPid = NULL;
    char command[1024];

    switch (fdwControl)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        if (ServiceStatus.dwCurrentState != SERVICE_RUNNING)
        {
            break;
        }

        ServiceStatus.dwControlsAccepted = 0;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWaitHint = 0;
        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCheckPoint = 4;

        dwPid = GetProcessId(pi.hProcess);
        if (dwPid)
        {
            sprintf_s(command, "taskkill /f /t /pid %lu", dwPid);
            system(command);
        }

        if (pi.hProcess)
        {
            CloseHandle(pi.hProcess);
            pi.hProcess = NULL;
        }

        if (pi.hThread)
        {
            CloseHandle(pi.hThread);
            pi.hThread = NULL;
        }

        if (hFile)
        {
            CloseHandle(hFile);
            hFile = NULL;
        }
        break;
    default:
        return;
    };

    if (!SetServiceStatus(hServiceStatusHandle, &ServiceStatus))
    {
        DWORD nError = GetLastError();
    }
}
