// TestConsole.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <cstdio>
#include <windows.h>
#include <detours.h>
#pragma comment(lib,"detours.lib")

#define MESSAGEBOXA 1
#define MESSAGEBOXW 2
#define CREATEFILE 3
#define WRITEFILE 4
#define READFILE 5
#define HEAPCREATE 6
#define HEAPDESTORY 7
#define HEAPFREE 8
#define REGCREATEKEYEX 9
#define REGSETVALUEEX 10
#define REGOPENKEYEX 11
#define REGDELETEVALUE 12

using namespace std;

int main(int argc, char* argv[])
{
    wchar_t fileName[256] = L"";
    MultiByteToWideChar(CP_ACP, 0, argv[0], strlen(argv[0]), fileName, sizeof(fileName));
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    WCHAR DirPath[MAX_PATH + 1];
    wcscpy_s(DirPath, MAX_PATH, L"D:\\hook\\InjectDll\\Debug");
    char DLLPath[MAX_PATH + 1] = "D:\\hook\\InjectDll\\Debug\\InjectDll.dll";
    WCHAR EXE[MAX_PATH + 1] = { 0 };
    wcscpy_s(EXE, MAX_PATH, fileName);
    if (DetourCreateProcessWithDllEx(EXE, NULL, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, NULL, DirPath, &si, &pi, DLLPath, NULL)) {
        ResumeThread(pi.hThread);
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    else {
        char error[100];
        sprintf_s(error, "%d", GetLastError());
    }


    return 0;
}



