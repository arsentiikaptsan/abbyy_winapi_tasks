#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "CEllipseWindow.hpp"

#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4

class COverlappedWindow {
public:
	COverlappedWindow() : childWindows(4) {
		windowHandle = NULL;
	};
	~COverlappedWindow() {}

	static bool registerClass() {
		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(windowClass));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = COverlappedWindow::windowProc;
		windowClass.hInstance = GetModuleHandle(0);
		windowClass.lpszClassName = L"OverlappedWindow";
		return (RegisterClassEx(&windowClass) != 0);
	}

	bool create() {
		CreateWindowEx(0, L"OverlappedWindow", L"Parent Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
		return (windowHandle != 0);
	}

	void show(int cmdShow) {
		ShowWindow(windowHandle, cmdShow);
		for (unsigned int i = 0; i < childWindows.size(); i++) {
			childWindows[i].show(cmdShow);
		}
	}

protected:
	void onDestroy() {
		PostQuitMessage(0);
	}

    void onNCCreate(HWND handle) {
		windowHandle = handle;
	}

    void onCreate() {
		CEllipseWindow::registerClass();
		for( int i = 0; i < childWindows.size(); i++ ){
			childWindows[i].create(windowHandle);
		}
	}

	void onKey(int direction) {
		int focus = getFocusedWindow();
		if (focus == -1) {
			return;
		}
		int newFocus = focus;
		switch (direction)
		{
			case UP: 
			{
				newFocus = focus > 1 ? focus - 2 : focus;
				break;
			}
			case DOWN: 
			{
				newFocus = focus < 2 ? focus + 2 : focus;
				break;
			}
			case RIGHT: 
			{
				newFocus = focus % 2 ? focus : focus + 1;
				break;
			}
			case LEFT: 
			{
				newFocus = focus % 2 ? focus - 1 : focus;
				break;
			}
		}

		SetFocus(childWindows[newFocus].getHandle());
	}

    void OnSize() {
		RECT rect;
		int middleX, middleY, nWidth, nHeight;
		GetClientRect(windowHandle, &rect);
		middleX = (rect.left + rect.right) / 2;
		middleY = (rect.top + rect.bottom) / 2;
		nWidth = (rect.right - rect.left) / 2;
		nHeight = (rect.bottom - rect.top) / 2;
		SetWindowPos(childWindows[0].getHandle(), HWND_TOP, rect.left, rect.top, nWidth, nHeight, 0);
		SetWindowPos(childWindows[1].getHandle(), HWND_TOP, middleX, rect.top, nWidth, nHeight, 0);
		SetWindowPos(childWindows[2].getHandle(), HWND_TOP, rect.left, middleY, nWidth, nHeight, 0);
		SetWindowPos(childWindows[3].getHandle(), HWND_TOP, middleX, middleY, nWidth, nHeight, 0);
		for (int i = 0; i < childWindows.size(); i++) {
			RECT rect;
			GetClientRect(childWindows[i].getHandle(), &rect);
			InvalidateRect(childWindows[i].getHandle(), &rect, TRUE);
		}
	}

private:
	HWND windowHandle;
	std::vector<CEllipseWindow> childWindows;

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_NCCREATE:
			{
				COverlappedWindow* window = (COverlappedWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
				SetLastError(0);
				SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
				if( GetLastError() != 0 ) {
					return GetLastError();
				}
				window->onNCCreate(handle);
				break;
			}
			case WM_CREATE:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onCreate();
				break;
			}
			case WM_KEYDOWN:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				switch (wParam) 
				{
					case VK_UP:
					{
						window->onKey(UP);
						break;
					}
					case VK_DOWN:
					{
						window->onKey(DOWN);
						break;
					}
					case VK_RIGHT:
					{
						window->onKey(RIGHT);
						break;
					}
					case VK_LEFT:
					{
						window->onKey(LEFT);
						break;
					}
				}
				break;
			}
			case WM_SIZE:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->OnSize();
				break;
			}
			case WM_DESTROY:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onDestroy();
				break;
			}
			default:
				break;
		}
		return DefWindowProc(handle, message, wParam, lParam);
	}

	int getFocusedWindow() {
		HWND focus = GetFocus();
		for (int i = 0; i < childWindows.size(); i++) {
			if (focus == childWindows[i].getHandle()) {
				return i;
			}
		}
		return -1;
	}
};
