#ifndef _X_WIN_H
#define _X_WIN_H

#include <Windows.h>
#define SetWindowOnTop(hWnd, onTop)	::SetWindowPos(hWnd,onTop?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE)


#endif // _X_WIN_H