#include <Windows.h>
#include "PingPongWindow.hpp"


int WINAPI wWinMain( HINSTANCE instance, HINSTANCE prevInstance, LPWSTR commandLine, int cmdShow ) 
{
	if( !PingPongWindow::registerClass() ) {
		return -1;
	}
	PingPongWindow window;
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