#ifndef _X_MAINWINDOW_HPP
#define _X_MAINWINDOW_HPP

#include "XWindow.hpp"
#include "XMenu.hpp"
#include "XToolBar.hpp"
#include "XStatusBar.hpp"

class XMainWindow :public XWindow{
public:
	XMainWindow(const XString& windowTitle=XString())
		:_menuBar(nullptr), _toolBar(nullptr), _statusBar(nullptr)
	{
		XWindow::createWindowEx(windowTitle, WS_OVERLAPPEDWINDOW, nullptr);
		move(GetSystemMetrics(SM_CXSCREEN) / 3, GetSystemMetrics(SM_CYSCREEN) / 3);
	}
	virtual ~XMainWindow(){}

	void setIcon(const XIcon& icon){ XWidget::setIcon(icon); }
	XMenuBar* menuBar(){
		if (_menuBar == nullptr){
			_menuBar = new XMenuBar(this);
			SetMenu(_hWnd, _menuBar->hMenu());
		}
		return _menuBar; 
	}
	XToolBar* toolBar(){ 
		if (_toolBar == nullptr)
			_toolBar = new XToolBar(this);
		return _toolBar; 
	}
	XStatusBar* statusBar(){ 
		if (_statusBar == nullptr)
			_statusBar = new XStatusBar(this);
		return _statusBar; 
	}



protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			::PostQuitMessage(0);
			break;
		}

		return XWindow::WndProc(message, wParam, lParam);
	}
	XSize layoutSizeHint(){ 
		RECT rt;
		::GetClientRect(_hWnd, &rt);
		return XSize(rt.right - rt.left, rt.bottom - rt.top);
	}

	
	void resizeEvent(XResizeEvent* event){ 
		if (_toolBar)	_toolBar->update();
		if (_statusBar)	_statusBar->update();
	}
private:
	XMenuBar* _menuBar;
	XToolBar* _toolBar;
	XStatusBar*	_statusBar;
};


#endif //_X_MAINWINDOW_HPP

