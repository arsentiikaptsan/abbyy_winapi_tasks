#include "NamesConvention.h"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <fstream>
#include <codecvt>
#include <utility>

#define DEFAULT_NUM_OF_PROCESSES 4

std::wstring readFile(wchar_t* filename)
{
	HANDLE sourceFile = CreateFile(filename, GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (sourceFile == NULL) {
		reportError(GetLastError());
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

void writeToFile(wchar_t* filename, std::wstring strToWrite) {
	HANDLE sourceFile = CreateFile(filename, GENERIC_WRITE, 0 , NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (sourceFile == NULL) {
		reportError(GetLastError());
	}
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = WriteFile( 
					sourceFile,
					strToWrite.c_str(),
					strToWrite.length() * sizeof(wchar_t),
                    &dwBytesWritten,
                    NULL);
	CloseHandle(sourceFile);
}

std::vector<int> countWordsAndSplitText(const wchar_t* text, int size) {
	int spaces_num = 0;
	std::vector<int> spacesLocations(1, 0);
	int i = 0;
	while (iswspace(text[i])) {
		i++;
	}

	bool isLastCharSpace = false;
	for (; i < size; i++) {
		if (iswspace(text[i])) {
			if (isLastCharSpace) {
				continue;
			}
			spaces_num++;
			spacesLocations.push_back(i);
			isLastCharSpace = true;
		} else {
			isLastCharSpace = false;
		}
	}
	if (!isLastCharSpace) {
		spacesLocations.push_back(i);
	}
	return spacesLocations;
}

void closeProcesses(std::vector<PROCESS_INFORMATION>::iterator first, std::vector<PROCESS_INFORMATION>::iterator last) {
	int N = last - first;
	HANDLE* handles = new HANDLE[N];
	for (int j = 0; j < N; j++) {
		handles[j] = (*(first + j)).hProcess;
	}
	WaitForMultipleObjects(N, handles, TRUE, INFINITE);
	delete[] handles;
}

void prepareDataForWorker(PROCESS_INFORMATION process, std::pair<wchar_t*, int> distribution, HANDLE& onReadyEvent, HANDLE& onTaskIsDoneEvent, HANDLE& mappedFile, PVOID& mappedFileText) {
	std::wstring onReadyEventName = (std::wstring(ON_READY_FOR_PROCESSING_EVENT) + std::to_wstring(process.dwProcessId));
	onReadyEvent = CreateEvent(NULL, FALSE, FALSE, onReadyEventName.c_str());
	//printf("%d", GetLastError());

	std::wstring onTaskIsDoneEventName = (std::wstring(ON_TASK_IS_DONE) + std::to_wstring(process.dwProcessId));
	onTaskIsDoneEvent = CreateEvent(NULL, FALSE, FALSE, onTaskIsDoneEventName.c_str());

	std::wstring fileName = (std::wstring(MAPPED_FILE_NAME) + std::to_wstring(process.dwProcessId));
	mappedFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, DEFAULT_MAPPED_FILE_SIZE, fileName.c_str());

	mappedFileText = MapViewOfFile(mappedFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	wcsncpy((wchar_t*)mappedFileText, distribution.first, distribution.second);
	((wchar_t*)mappedFileText)[distribution.second] = 0;
}

std::vector<std::pair<wchar_t*, int>> splitTextForWorkers(PVOID sourceText, int sourceFileSize) {
	std::vector<int> spacesLocations = countWordsAndSplitText((wchar_t*)sourceText, sourceFileSize / sizeof(wchar_t));
	int wordsNum = spacesLocations.size() - 1;
	int numOfProcesses = min(wordsNum, DEFAULT_NUM_OF_PROCESSES);
	std::vector<std::pair<wchar_t*, int>> distribution(numOfProcesses);
	int wordsForProcess = wordsNum / numOfProcesses;
	for (int i = 0; i < numOfProcesses; i++) {
		int upperBound;
		if (i == numOfProcesses - 1) {
			upperBound = spacesLocations.back();
		} else {
			upperBound = spacesLocations[(i+1)*wordsForProcess];
		}
		distribution[i] = std::make_pair(&((wchar_t*)sourceText)[spacesLocations[i*wordsForProcess]], upperBound - spacesLocations[i*wordsForProcess]);
	}
	return distribution;
}

int __cdecl wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {
	if (argc != 4) {
		std::cout << "Wrong arguments number" << std::endl;
		return 1;
	}

	//все файлы должны быть в UTF-16
	wchar_t* dictFileName = argv[2];
	wchar_t* sourceFileName = argv[1];
	wchar_t* outputFileName = argv[3];

	std::wstring cmndLineString = readFile(dictFileName);
	wchar_t* dictForCommandLine = new wchar_t[cmndLineString.length() + 1];
	wcscpy(dictForCommandLine, cmndLineString.c_str());

	HANDLE sourceFile = CreateFile(sourceFileName, GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (sourceFile == NULL) {
		reportError(GetLastError());
		delete[] dictForCommandLine;
		return 1;
	}
	DWORD sourceFileSize = GetFileSize(sourceFile, NULL);
	HANDLE sourceFileMap = CreateFileMapping(sourceFile, NULL, PAGE_READONLY, 0, sourceFileSize, NULL);
	PVOID sourceText = MapViewOfFile(sourceFileMap, FILE_MAP_READ, 0, 0, 0);

	std::vector<std::pair<wchar_t*, int>> distribution = splitTextForWorkers((wchar_t*)sourceText + 1, sourceFileSize - sizeof(wchar_t));
	int numOfProcesses = distribution.size();

	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

	HANDLE onClosure = CreateEvent(NULL, TRUE, FALSE, ON_CLOSE_EXECUTOR_EVENT);
	//ResetEvent(onClosure);

	std::vector<PROCESS_INFORMATION> processes(numOfProcesses);
	for (int i = 0; i < numOfProcesses; i++) {
		ZeroMemory( &processes[i], sizeof(processes[i]) );
		BOOL res = CreateProcess( L"C:/Users/arsentii/Documents/Visual Studio 2012/Projects/Task5/Debug/CleanerUnit.exe",   
								dictForCommandLine,       
								NULL,           // Process handle not inheritable
								NULL,           // Thread handle not inheritable
								FALSE,          // Set handle inheritance to FALSE
								0,              // No creation flags
								NULL,           // Use parent's environment block
								NULL,           // Use parent's starting directory 
								&si,            // Pointer to STARTUPINFO structure
								&processes[i] );
		CloseHandle(processes[i].hProcess);
		CloseHandle(processes[i].hThread);
		if (!res) {
			reportError(GetLastError());
			SetEvent(onClosure);

			closeProcesses(processes.begin(), processes.begin() + i);
			CloseHandle(onClosure);
			UnmapViewOfFile(sourceFileMap);
			delete[] dictForCommandLine;

			return 1;
		}
	}

	std::vector<HANDLE> onReadyEvents(numOfProcesses);
	std::vector<HANDLE> onTaskIsDoneEvents(numOfProcesses);
	std::vector<HANDLE> mappedFiles(numOfProcesses);
	std::vector<PVOID> mappedFilesText(numOfProcesses);
	for (int i = 0; i < numOfProcesses; i++) {
		prepareDataForWorker(processes[i], distribution[i], onReadyEvents[i], onTaskIsDoneEvents[i], mappedFiles[i], mappedFilesText[i]);

		std::wstring debugStr((wchar_t*)mappedFilesText[i]);
		int res = SetEvent(onReadyEvents[i]);
		//printf("%d", res);
	}

	std::wstring debugStr(dictForCommandLine);
	UnmapViewOfFile(sourceFileMap);

	WaitForMultipleObjects(numOfProcesses, &onTaskIsDoneEvents[0], TRUE, INFINITE);

	//SetEvent(onClosure);
	closeProcesses(processes.begin(), processes.end());

	std::wstring finalText;
	for (int i = 0; i < numOfProcesses; i++) {
		finalText += (wchar_t*)mappedFilesText[i];
		if (i < numOfProcesses - 1) {
			finalText += L" ";
		}
	}
	boost::trim(finalText);
	
	writeToFile(outputFileName, finalText);
	unmapFiles(mappedFiles);
	delete[] dictForCommandLine;
	return 0;
}