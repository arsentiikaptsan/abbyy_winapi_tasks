#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <codecvt>


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
    if (sourceFile == NULL) {
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

void reportError(DWORD errorCode, std::string message = std::string()) 
{
    std::cerr << "Error " << message << std::to_string(errorCode);
}

int wmain(int argc, wchar_t* argv[]) 
{    
    if( argc < 2 ) {
        return 1;
    }
    std::wstring fileText = readFile(argv[1]);

    typedef int (__cdecl *MYPROC)(const wchar_t*); 
 
    HINSTANCE hinstLib; 
    MYPROC procWordsCount; 
    int wordsNum;
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 
 
    hinstLib = LoadLibrary(TEXT("DLLPart.dll")); 
 
    if( hinstLib != NULL ) { 
        procWordsCount = (MYPROC) GetProcAddress(hinstLib, "CountWords");
        //reportError(GetLastError());
 
        if( NULL != procWordsCount ) {
            fRunTimeLinkSuccess = TRUE;
            wordsNum = (procWordsCount) (fileText.c_str()); 
        }
 
        fFreeResult = FreeLibrary(hinstLib); 
    } 

    if( !fRunTimeLinkSuccess ) {
        printf("Linking dll failed\n"); 
        return -1;
    }

    showMessage(wordsNum);

    return 0;
}