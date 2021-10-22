#include <iostream>
#include <string>
#include "stdio.h"
#include "tchar.h"
#include <atlstr.h>
using namespace std;

#define NOMINMAX 1
#define byte win_byte_override
#include "Windows.h"

#define SERVICE_NAME TEXT("")
SERVICE_STATUS ServiceStatus = {0};
SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL;

STARTUPINFO si;
PROCESS_INFORMATION pi;

void WINAPI ServiceMain(int argc, char **argv);
void WINAPI ServiceHandler(DWORD fdwControl);

string pCommand = "";
string pCwd = "";

int main(int argc, char **argv)
{

    char **temp = argv;

    string command = "";
    string cwd = "";
    string type = "";
    char line = '-';

    while (*temp != NULL)
    {
        string a = *temp;
        ++temp;

        bool isOpt = false;

        if (a.length() > 2)
        {
            char a_0 = a.at(0);
            char a_1 = a.at(1);

            if (a_0 == line && a_1 == line)
            {
                isOpt = true;
            }
        }

        if (isOpt)
        {
            if (a == "--cmd")
            {
                type = "cmd";
            }
            else if (a == "--cwd")
            {
                type = "cwd";
            }
            else
            {
                type = "";
            }
        }
        else
        {
            if (type == "cmd")
            {
                if (command.length() == 0)
                {
                    command = a;
                }
                else
                {
                    command += " " + a;
                }
            }
            else if (type == "cwd")
            {
                if (cwd.length() == 0)
                {
                    cwd = a;
                }
                else
                {
                    cwd += " " + a;
                }
            }
        }
    }

    pCommand = command;
    pCwd = cwd;

    // 启动服务
    SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = SERVICE_NAME;
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}

// ---------------服务主函数----------------
void WINAPI ServiceMain(int argc, char **argv)
{
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

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

    // 添加运行任务
    string workdir = pCwd;
    string cmdstr = pCommand;
    LPSTR lpcmdstr = &cmdstr.front();
    if (CreateProcessA(NULL, lpcmdstr, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, workdir.length() ? workdir.c_str() : NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
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
    }

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

        TerminateProcess(pi.hProcess, uExitCode);
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
        break;
    default:
        return;
    };

    if (!SetServiceStatus(hServiceStatusHandle, &ServiceStatus))
    {
        DWORD nError = GetLastError();
    }
}
