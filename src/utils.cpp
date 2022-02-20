#include <Windows.h>
#include <stdio.h>
#include <clocale>

#include "./base.h"

#define byte win_byte_override
#define BUFSIZE 4096

std::wstring ToUtf16(std::string str, unsigned int acp = CP_UTF8)
{
    std::wstring ret;
    int len = MultiByteToWideChar(acp, 0, str.c_str(), str.length(), NULL, 0);
    if (len > 0)
    {
        ret.resize(len);
        MultiByteToWideChar(acp, 0, str.c_str(), str.length(), &ret[0], len);
    }
    return ret;
}

bool execCmdline(PROCESS_INFORMATION &pi, HANDLE &hFile, const string &cmdline, bool waitComplete, const string &workdir, LogOptions &logOptions, unsigned int acp)
{
    STARTUPINFOW si;

    ZeroMemory(&si, sizeof(STARTUPINFOW));
    si.cb = sizeof(STARTUPINFOW);
    ZeroMemory(&pi, sizeof(pi));

    HANDLE m_hChildStd_OUT_Rd = NULL;
    HANDLE m_hChildStd_OUT_Wr = NULL;

    // handle log
    if (logOptions.isLog)
    {
        // open file
        if (1)
        {
            SECURITY_ATTRIBUTES sa;
            ZeroMemory(&sa, sizeof(sa));
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.lpSecurityDescriptor = NULL;
            sa.bInheritHandle = TRUE;

            string logpathStr = logOptions.filepath;
            wstring logpahtWstr = ToUtf16(logpathStr, acp);
            LPCWSTR lpLogpathStr = &logpahtWstr.front();

            // get file size
            long long longFileSize = 0;
            hFile = CreateFileW(
                lpLogpathStr,
                FILE_APPEND_DATA,
                FILE_SHARE_WRITE | FILE_SHARE_READ,
                &sa,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            LARGE_INTEGER lpFileSize;
            bool isGetSize = GetFileSizeEx(hFile, &lpFileSize);
            CloseHandle(hFile);
            hFile = NULL;
            if (isGetSize)
            {
                longFileSize = lpFileSize.QuadPart;
            }

            // check overflow
            bool overLimit = false;
            if (logOptions.limit && logOptions.limit <= longFileSize)
            {
                overLimit = true;
                longFileSize = 0;
            }

            // open file
            hFile = CreateFileW(
                lpLogpathStr,
                FILE_APPEND_DATA,
                FILE_SHARE_WRITE | FILE_SHARE_READ,
                &sa,
                !overLimit && logOptions.isAppend ? OPEN_ALWAYS : CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            // write head to file
            if (logOptions.isLogHead)
            {
                SYSTEMTIME lt;
                GetLocalTime(&lt);
                char headStr[200];
                sprintf_s(headStr, "%s    [TIME %02d-%02d-%02d %02d:%02d:%02d.%02d]\n", longFileSize ? "\n" : "", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);

                DWORD bytesWritten;
                WriteFile(hFile, headStr, strlen(headStr), &bytesWritten, NULL);
            }
        }

        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput = NULL;
        si.hStdError = hFile;
        si.hStdOutput = hFile;
    }

    // create process
    string cmdstr = cmdline;
    string workstr = workdir;

    wstring cmdwstr = ToUtf16(cmdstr, acp);
    LPWSTR lpcmdstr = &cmdwstr.front();

    wstring workwstr = ToUtf16(workstr, acp);
    LPCWSTR lpworkstr = workwstr.c_str();

    if (!CreateProcessW(NULL, lpcmdstr, NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, NULL, workdir.empty() ? NULL : lpworkstr, &si, &pi))
    {
        return false;
    }

    if (waitComplete)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
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

    return true;
}

string getCurExePath()
{
    char *tr = nullptr;
    _get_pgmptr(&tr);

    string r = tr;

    return r;
}

string getCurExeFileName()
{
    path p = getCurExePath();
    string name = p.filename().string();
    return name;
}

string getCurExeDir()
{
    string name = getCurExePath();
    path p = name;

    path pp = p.parent_path();
    string r = pp.string();

    return r;
}
