#pragma once
#include <windows.h>
#include <string>

class COverlappedWindow {
public:
	COverlappedWindow() : tick(10), velocity(0.01), R(50), rX(30), rY(20) {
		windowHandle =0;
		time = 0; 
		timerId = 0;
	};
	~COverlappedWindow() {}

	static bool registerClass() {
		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(windowClass));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = COverlappedWindow::windowProc;
		windowClass.hInstance = GetModuleHandle(0);
		windowClass.lpszClassName = L"OverlappedWindow";
		windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		return (RegisterClassEx(&windowClass) != 0);
	}

	bool create() {
		CreateWindowEx(0, L"OverlappedWindow", L"My Window", WS_EX_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), this);
		return (windowHandle != 0);
	}

	void show(int cmdShow) {
		ShowWindow(windowHandle, cmdShow);
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
		timerId = SetTimer(windowHandle, 0, tick, 0);
	}

    void onPaint() {
		RECT clientRect;
		PAINTSTRUCT paintStruct;
		HDC windowDC = BeginPaint(windowHandle, &paintStruct);
		GetClientRect(windowHandle, &clientRect);
		HDC displayBufferDC = CreateCompatibleDC(windowDC);
		HBITMAP displayBuffer = CreateCompatibleBitmap(windowDC, clientRect.right - clientRect.left, 
			clientRect.bottom - clientRect.top);
		HGDIOBJ oldDisplayBuffer = SelectObject(displayBufferDC, displayBuffer);
		int x = (clientRect.left + clientRect.right) / 2 + (int)(R * cos(time * velocity));
		int y = (clientRect.top + clientRect.bottom) / 2 + (int)(R * sin(time * velocity));
		FillRect(displayBufferDC, &clientRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		drawEllipse(displayBufferDC, x, y);
		BitBlt(windowDC, clientRect.left, clientRect.top, clientRect.right - clientRect.left, 
			clientRect.bottom - clientRect.top, displayBufferDC, 0, 0, SRCCOPY);
		SelectObject(displayBufferDC, oldDisplayBuffer);
		DeleteObject(displayBuffer);
		DeleteDC(displayBufferDC);
		EndPaint(windowHandle, &paintStruct);
	}

    void onTimer() {
		RECT rect;
		GetClientRect(windowHandle, &rect);
		time += tick;
		InvalidateRect(windowHandle, &rect, TRUE);
	}

private:
	HWND windowHandle;
    UINT_PTR timerId;
    double time;

    const double tick;//milliseconds
	const double velocity;//rad per millisecond
	const int R;
    const int rX;
	const int rY;

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
				return DefWindowProc(handle, message, wParam, lParam);
			}
			case WM_CREATE:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onCreate();
				return DefWindowProc(handle, message, wParam, lParam);
			}
			case WM_DESTROY:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onDestroy();
				break;
			}
			case WM_PAINT:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onPaint();
				break;
			}
			case WM_TIMER:
			{
				COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onTimer();
				break;
			} 
			default:
				return DefWindowProc(handle, message, wParam, lParam);
		}
		return DefWindowProc(handle, message, wParam, lParam);
	}
    void drawEllipse(HDC targetDC, int x, int y) {
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		HBRUSH innBrush = CreateSolidBrush(RGB(255, 0, 0));
		HGDIOBJ oldPen = SelectObject(targetDC, pen);
		HGDIOBJ oldBrush = SelectObject(targetDC, innBrush);
		Ellipse(targetDC, x - rX, y - rY, x + rX, y + rY);
		SelectObject(targetDC, oldPen);
		SelectObject(targetDC, oldBrush);
		DeleteObject(innBrush);
		DeleteObject(pen);
	}
};
