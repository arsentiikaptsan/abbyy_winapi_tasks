#pragma once
#include <windows.h>
#include <string>

class CEllipseWindow {
public:
	CEllipseWindow() : tick(10), velocity(0.01), R(50), rX(30), rY(20) {
		windowHandle = 0;
		parentWindowHandle = 0;
		time = 0; 
		timerId = 0;
		isLastOnTimerCalledOnPaint = false;
	};

	~CEllipseWindow() {}

	static bool registerClass() {
		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(windowClass));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = CEllipseWindow::windowProc;
		windowClass.hInstance = GetModuleHandle(0);
		windowClass.lpszClassName = L"EllipseWindow";
		windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		return (RegisterClassEx(&windowClass) != 0);
	}

	bool create(HWND hWndParent) {
		parentWindowHandle = hWndParent;
		CreateWindowEx(0, L"EllipseWindow", L"Child Window", WS_CHILD | WS_BORDER,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndParent, 0, GetModuleHandle(0), this);
		return (windowHandle != 0);
	}

	void show(int cmdShow) {
		ShowWindow(windowHandle, cmdShow);
	}

	HWND getHandle() {
		return windowHandle;
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
		if ((GetFocus() == windowHandle) || isLastOnTimerCalledOnPaint) {
			RECT rect;
			GetClientRect(windowHandle, &rect);
			time += tick;
			InvalidateRect(windowHandle, &rect, TRUE);
			isLastOnTimerCalledOnPaint = true;
		}
		if (GetFocus() != windowHandle) {
			isLastOnTimerCalledOnPaint = false;
		}
	}

	void onLButtonDown() {
		SetFocus(windowHandle);
	}

private:
	HWND windowHandle;
	HWND parentWindowHandle;
    UINT_PTR timerId;
    double time;
	bool isLastOnTimerCalledOnPaint;

    double tick;//milliseconds
	double velocity;//rad per millisecond
	int R;
    int rX;
	int rY;

    static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_NCCREATE:
			{
				CEllipseWindow* window = (CEllipseWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
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
				CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onCreate();
				break;
			}
			case WM_DESTROY:
			{
				CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onDestroy();
				break;
			}
			case WM_KEYDOWN:
			{
				CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				SendMessage(window->parentWindowHandle, message, wParam, lParam);
				break;
			}
			case WM_PAINT:
			{
				CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onPaint();
				break;
			}
			case WM_TIMER:
			{
				CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onTimer();
				break;
			} 
			case WM_LBUTTONDOWN:
			{
				CEllipseWindow* window = (CEllipseWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
				window->onLButtonDown();
				break;
			}
			default:
				break;
		}
		return DefWindowProc(handle, message, wParam, lParam);
	}
    void drawEllipse(HDC targetDC, int x, int y) {
		HGDIOBJ oldPen, oldBrush;
		HBRUSH innBrush;
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		if (GetFocus() == windowHandle) {
			innBrush = CreateSolidBrush(RGB(255, 0, 0));
		}
		else {
			innBrush = CreateSolidBrush(RGB(0, 0, 255));
		}
		oldPen = SelectObject(targetDC, pen);
		oldBrush = SelectObject(targetDC, innBrush);
		Ellipse(targetDC, x - rX, y - rY, x + rX, y + rY);
		SelectObject(targetDC, oldPen);
		SelectObject(targetDC, oldBrush);
		DeleteObject(innBrush);
		DeleteObject(pen);
	}
};