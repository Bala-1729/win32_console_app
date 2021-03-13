#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include "atlstr.h" 
#include <iostream>
#include <map>
using namespace std;
#pragma comment(lib, "User32.lib")

void DisplayErrorBox(LPTSTR lpszFunction);

int _tmain(int argc, TCHAR* argv[])
{
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    _tprintf(TEXT("\nTarget directory is %s\n\n"), argv[1]);

    StringCchCopy(szDir, MAX_PATH, argv[1]);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    hFind = FindFirstFile(szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        DisplayErrorBox(LPTSTR("FindFirstFile"));
        return dwError;
    }

    do
    {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            CHAR *token, *temp = strtok(ffd.cFileName, ".");
            token = temp;
            while (temp != NULL)
            {
                token = temp;
                temp = strtok(NULL, ".");
            }
            cout<<token<<"  "<<filesize.QuadPart* 0.00097656 <<" Kilobytes"<<endl;
        }
    } while (FindNextFile(hFind, &ffd) != 0);
    
    hFind = FindFirstFile(szDir, &ffd);

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            TCHAR* path1 = argv[1], * path2 = ffd.cFileName, path[260];
            if (strcmp(path2, ".") && strcmp(path2, ".."))
            {
                _tprintf(TEXT("\nEntering subfolder:\n"));
                _stprintf(path, _T("%s\\%s"), path1, path2);
                TCHAR* temp = argv[1];
                argv[1] = path;
                _tmain(2, argv);
                argv[1] = temp;
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);
      
    

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        DisplayErrorBox(LPTSTR("FindFirstFile"));
    }

    FindClose(hFind);
    return dwError;
}


void DisplayErrorBox(LPTSTR lpszFunction)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and clean up

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}