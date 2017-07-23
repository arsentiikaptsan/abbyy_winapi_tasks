#include <windows.h>
#include "COverlappedWindow.hpp"


int WINAPI wWinMain( HINSTANCE instance, HINSTANCE prevInstance, LPWSTR commandLine, int cmdShow ) 
{
	if( !COverlappedWindow::registerClass() ) {
		return -1;
	}
	COverlappedWindow window;
	if( !window.create() ) {
		return -1;
	}
	window.show(cmdShow);
	MSG message;
	BOOL getMessageResult = 0;
	while( (getMessageResult = GetMessage( &message, 0, 0, 0 )) != 0 ) {
		if( getMessageResult == -1 ) {
			return -1;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return 0;
}