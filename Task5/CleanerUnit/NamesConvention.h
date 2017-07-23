#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

#define ON_READY_FOR_PROCESSING_EVENT L"Global\\Ready"
#define ON_CLOSE_EXECUTOR_EVENT L"Global\\Close"
#define MAPPED_FILE_NAME L"Hui"
#define ON_TASK_IS_DONE L"Global\\Done"


#define DEFAULT_BUFFER_SIZE 100
#define DEFAULT_MAPPED_FILE_SIZE 4*1024

void reportError(DWORD errorCode, std::string message = std::string()) {
	std::cerr << "Error " << message << std::to_string(errorCode);
}

void closeHandles(std::vector<HANDLE> handles) {
	for (int i = 0; i < handles.size(); i++) {
		CloseHandle(handles[i]);
	}
}

void unmapFiles(std::vector<HANDLE> handles) {
	for (int i = 0; i < handles.size(); i++) {
		UnmapViewOfFile(handles[i]);
	}
}