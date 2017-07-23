#include "NamesConvention.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>


//text with 0 in the end
void cleanTextInMappedFile(PVOID text, int dictSize, wchar_t* dict[]) {
	//wprintf(L"HUI");
	std::wstring textCopy((wchar_t*)text);
	textCopy.push_back(L' ');
	textCopy.insert(0, L" ");
	for (int i = 0; i < dictSize; i++) {
		std::wstring pattern = L" " + std::wstring(dict[i]) + L" ";
		boost::replace_all(textCopy, pattern, L" ");
		/*size_t pos = 0;
		while ((pos = textCopy.find(pattern, pos)) != std::wstring::npos) {
			textCopy.replace(pos, pattern.length(), L" ");
			 pos += 1;
		}*/
	}
	boost::trim(textCopy);
	wcscpy((wchar_t*)text, textCopy.c_str());
	return;
}


int __cdecl wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {
	DWORD processID = GetCurrentProcessId();
	std::wstring onReadyForProcessingEventName = (std::wstring(ON_READY_FOR_PROCESSING_EVENT) + std::to_wstring(processID));
	LPCTSTR onCloseExecutorEventName = ON_CLOSE_EXECUTOR_EVENT;
	std::wstring mappedFileName = (std::wstring(MAPPED_FILE_NAME) + std::to_wstring(processID));
	std::wstring onTaskIsDoneEventName = (std::wstring(ON_TASK_IS_DONE) + std::to_wstring(processID));

	HANDLE onReadyForProcessingEvent = CreateEvent(NULL, FALSE, FALSE, onReadyForProcessingEventName.c_str());
	if (onReadyForProcessingEvent == NULL) {
		reportError(GetLastError(), std::string("1"));
		return 1;
	}
	
	HANDLE onCloseExecutorEvent = CreateEvent(NULL, TRUE, FALSE, onCloseExecutorEventName);
	if (onCloseExecutorEvent == NULL) {
		reportError(GetLastError(), std::string("2"));
		return 1;
	}
	HANDLE onTaskIsDoneEvent = CreateEvent(NULL, FALSE, FALSE, onTaskIsDoneEventName.c_str());
	if (onTaskIsDoneEvent == NULL) {
		reportError(GetLastError(), std::string("3"));
		return 1;
	}
	HANDLE mappedFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, DEFAULT_MAPPED_FILE_SIZE, mappedFileName.c_str());
	if (mappedFile == NULL) {
		reportError(GetLastError(), std::string("4"));
		return 1;
	}

	HANDLE events[2];
	events[0] = onReadyForProcessingEvent;
	events[1] = onCloseExecutorEvent;
	DWORD catchedEvent = WaitForMultipleObjects(2, events, FALSE, INFINITE);
	switch (catchedEvent) {
		case WAIT_OBJECT_0 + 0: 
		{
			//wprintf(L"HUI1");
			PVOID fileText = MapViewOfFile(mappedFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
			cleanTextInMappedFile(fileText, argc, &argv[0]);
			BOOL res = SetEvent(onTaskIsDoneEvent);
			UnmapViewOfFile(mappedFile);
			if (!res) {
				reportError(GetLastError());
				return 1;
			}
			return 0;
		}

		case WAIT_OBJECT_0 + 1:
			//wprintf(L"HUI2");
			return 0;
	}
}