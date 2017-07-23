#pragma once
#include <windows.h>
#include <string>
#include "Presenter.h"

#define FRAMES_PER_SECOND 50


class PingPongWindow {
public:
	PingPongWindow() : tickDuraion(1.0 / (double)FRAMES_PER_SECOND) {
		windowHandle = 0;
		presenter = new Presenter(this);
	};

	~PingPongWindow() {
		delete presenter;
	}

	static bool registerClass() {
		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(windowClass));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = PingPongWindow::windowProc;
		windowClass.hInstance = GetModuleHandle(0);
		windowClass.lpszClassName = L"PingPongWindow";
		windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		return (RegisterClassEx(&windowClass) != 0);
	}

	bool create() {
		int windowParams[2] = {SCALE_FACTOR * LAYOUT_LENGTH, SCALE_FACTOR * LAYOUT_HEIGHT};
		
		RECT desiredRect;
		desiredRect.top = 0;
		desiredRect.left = 0;
		desiredRect.bottom = SCALE_FACTOR * LAYOUT_HEIGHT;
		desiredRect.right = SCALE_FACTOR * LAYOUT_LENGTH;
		AdjustWindowRectEx(&desiredRect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE, NULL);

		CreateWindowEx(0, L"PingPongWindow", L"Main Window", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, desiredRect.right - desiredRect.left, desiredRect.bottom - desiredRect.top, 0, 0, GetModuleHandle(0), this);
		return (windowHandle != 0);
	}

	void show(int cmdShow) {
		ShowWindow(windowHandle, cmdShow);
	}

	HDC startDrawing() {
		RECT clientRect;
		windowDC = BeginPaint(windowHandle, &paintStruct);
		GetClientRect(windowHandle, &clientRect);
		displayBufferDC = CreateCompatibleDC(windowDC);
		displayBuffer = CreateCompatibleBitmap(windowDC, clientRect.right - clientRect.left, 
			clientRect.bottom - clientRect.top);
		oldDisplayBuffer = SelectObject(displayBufferDC, displayBuffer);
		return displayBufferDC;
	}

	void endDrawing() {
		RECT clientRect;
		GetClientRect(windowHandle, &clientRect);
		BitBlt(windowDC, clientRect.left, clientRect.top, clientRect.right - clientRect.left, 
			clientRect.bottom - clientRect.top, displayBufferDC, 0, 0, SRCCOPY);
		SelectObject(displayBufferDC, oldDisplayBuffer);
		DeleteObject(displayBuffer);
		DeleteDC(displayBufferDC);
		EndPaint(windowHandle, &paintStruct);
	}

	void getClientRectangle(RECT& rect) {
		GetClientRect(windowHandle, &rect);
	}

	void onGameOver() {
		DestroyWindow(windowHandle);
	}

	pingpong::PressedKey getPressedKey() {
		bool leftIsPressed = false, rightIsPressed = false;
		if (GetKeyState(VK_LEFT) & 0x8000) {
			leftIsPressed = true;
		}
		if (GetKeyState(VK_RIGHT) & 0x8000) {
			rightIsPressed = true;
		}

		if (leftIsPressed && rightIsPressed) {
			return pingpong::OTHER_KEY;
		}
		if (leftIsPressed) {
			return pingpong::KEY_LEFT;
		}
		if (rightIsPressed) {
			return pingpong::KEY_RIGHT;
		}
		return pingpong::NO_KEY;
	}

	double getTickDuration() {
		return tickDuraion;
	}

protected:
	void onDestroy() {
		KillTimer(windowHandle, timerId);
		PostQuitMessage(0);
	}

    void onNCCreate(HWND handle) {
		windowHandle = handle;
	}

    void onCreate() {
		timerId = SetTimer(windowHandle, 0, tickDuraion * 1e3, 0);
		presenter->onInitialShowup();
	}

    void onPaint() {
		//presenter->onInitialShowup();
		presenter->onPaint();
	}

    void onTimer() {
		presenter->onTick();
		RECT rect;
		GetClientRect(windowHandle, &rect);
		InvalidateRect(windowHandle, &rect, TRUE);
	}

private:
	HWND windowHandle;
    UINT_PTR timerId;

	const double tickDuraion;

	PAINTSTRUCT paintStruct;
	HDC windowDC;
	HDC displayBufferDC;
	HBITMAP displayBuffer;
	HGDIOBJ oldDisplayBuffer;

	Presenter* presenter;

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_NCCREATE:
			{
				PingPongWindow* window = (PingPongWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
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
				PingPongWindow* window = (PingPongWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onCreate();
				break;
			}
			case WM_DESTROY:
			{
				PingPongWindow* window = (PingPongWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onDestroy();
				break;
			}
			case WM_PAINT:
			{
				PingPongWindow* window = (PingPongWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onPaint();
				break;
			}
			case WM_TIMER:
			{
				PingPongWindow* window = (PingPongWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onTimer();
				break;
			} 

			default:
				break;
		}
		return DefWindowProc(handle, message, wParam, lParam);
	}
};