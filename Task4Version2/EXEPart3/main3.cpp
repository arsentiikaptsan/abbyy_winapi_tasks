#pragma comment(lib, "user32.lib")

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <codecvt>

#include "WordsCounter2.h"

void showMessage(int wordsNum) 
{
    std::wstring str = std::to_wstring(wordsNum);
    LPCWSTR message = str.c_str();
    MessageBox(
        NULL,
        message,
        (LPCWSTR)L"Account Details",
        MB_OK
    );
}

std::wstring readFile(const wchar_t* filename)
{
    HANDLE sourceFile = CreateFile(filename, GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if( sourceFile == NULL ) {
        return std::wstring();
    }
    DWORD sourceFileSize = GetFileSize(sourceFile, NULL);
    HANDLE sourceFileMap = CreateFileMapping(sourceFile, NULL, PAGE_READONLY, 0, sourceFileSize, NULL);
    PVOID sourceText = MapViewOfFile(sourceFileMap, FILE_MAP_READ, 0, 0, 0);
    std::wstring result(&((wchar_t*)sourceText)[1]);
    UnmapViewOfFile(sourceFileMap);
    CloseHandle(sourceFile);
    CloseHandle(sourceFileMap);
    return result;
}

int wmain(int argc, wchar_t* argv[]) 
{    
    if( argc < 2 ) {
        return 1;
    }
    std::wstring fileText = readFile(argv[1]);
    
    int wordsNum = CountWords(fileText.c_str());

    showMessage(wordsNum);

    return 0;
}