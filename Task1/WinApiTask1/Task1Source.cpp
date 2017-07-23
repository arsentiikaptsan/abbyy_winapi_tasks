#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <ctime>


LPVOID allocByReserve(int numOfBytes) {
	return VirtualAlloc(
                     NULL,                 // System selects address
                     numOfBytes, // Size of allocation
                     MEM_RESERVE,          // Allocate reserved pages
                     PAGE_NOACCESS);
}

LPVOID allocByCommitAndReserve(int numOfBytes, DWORD protect = PAGE_NOACCESS) {
	return VirtualAlloc(
                     NULL,                 // System selects address
                     numOfBytes, // Size of allocation
					 MEM_RESERVE | MEM_COMMIT,          // Allocate reserved pages
                     protect);
}


std::wstring getLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::wstring(); //No error message has been recorded

    wchar_t buf[256];
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);

    std::wstring message(buf);

    return message;
}

void foo(int* counter) {
	(*counter)++;
	foo(counter);
}

void goo(int* counter) {
	(*counter)++;
	int x,y,z;
	goo(counter);
}

VOID _tmain(VOID) {
	SYSTEM_INFO sSysInfo;

    GetSystemInfo(&sSysInfo);

    _tprintf (TEXT("This computer has page size %d.\n"), sSysInfo.dwPageSize);

    /////////// Ex 1.1 ////////////
	/*LPVOID ptr = allocByCommitAndReserve((int)2e11);
	std::wstring message = getLastErrorAsString();
	if (ptr == NULL) {
		std::wcout << GetLastError() << std::endl;
	} else {
		std::wcout << L"OK" << std::endl;
	}*/
	//////////result =  ERROR_NOT_ENOUGH_MEMORY////////////

	/////////// Ex 1.2 ////////////
	/*LPVOID ptr = allocByReserve((int)1e11);
	std::wstring message = getLastErrorAsString();
	if (ptr == NULL) {
		std::wcout << GetLastError() << std::endl;
	} else {
		std::wcout << L"OK" << std::endl;
	}*/

	////////// Ex 2 //////////////
	std::srand(unsigned(std::time(0)));
    int random_variable = std::rand();
	for (int i = 0; i < 1e6; i++) {
		int random_alloc_size = (8 * 1024 * 1024);
		DWORD page_guard_flag = (std::rand() % 2 == 0) ? PAGE_NOACCESS : PAGE_GUARD | PAGE_READONLY;
		LPVOID ptr = allocByCommitAndReserve(random_alloc_size, page_guard_flag);
		if (ptr == NULL) {
			system("pause");
			std::wcout << GetLastError() << std::endl;
			int h;
			std::cin >> h;
		}
	}

	///////// Ex 3 ///////////// res = 16m
	/*HANDLE handle = CreateEvent(NULL, TRUE, TRUE, NULL);
	int num_of_handlers = (int) 1e10;

	for (int i = 0; i < num_of_handlers; i++) {
		BOOL res = DuplicateHandle(GetCurrentProcess(), 
                    handle, 
                    GetCurrentProcess(),
                    NULL, 
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
		if (res == 0) {
			std::wcout << "Error " << GetLastError() << " handles number: " << i << std::endl;
			int h;
			std::cin >> h;
			break;
		}
	}*/

	///////// Ex 4 /////////////
	//int num_of_handlers = (int) 1e8;
	//HPEN* pens = new HPEN[num_of_handlers];

	//for (int i = 0; i < num_of_handlers; i++) {
	//	pens[i] = CreatePen(PS_SOLID, 0, RGB(0,255,0));
	//	//std::wcout << pens[i] << std::endl;
	//	if (pens[i] == NULL) {
	//		std::wcout << "Error " << GetLastError() << " pens number: " << i << std::endl;
	//		break;
	//	}
	//}
	//delete[] pens;

	///////// Ex 5 /////////// stack size aprox 1mb
	////////////////////////// counter = 4786 => 200b is one call
	///////////////////////// counter = 4108 for 3 local int vars
	/*int counter = 0;
	goo(&counter);*/
	
}