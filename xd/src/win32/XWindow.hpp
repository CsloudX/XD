#ifndef _X_WINDOW_HPP
#define _X_WINDOW_HPP

#include "XWidget.hpp"


class XWindow :public XWidget{
public:
	XWindow(const XString& windowName = XString(), XWidget* parent = nullptr) :XWidget(parent){
		X_UNUSED(windowName);
	}
	virtual ~XWindow(){}
	void setWindowTitle(const XString& title){ XWidget::setWindowText(title); }
protected:
	void createWindowEx(const XString& windowName, DWORD style, XWidget* parent,
		DWORD exStyle = 0, int x = 0, int y = 0, int width = 0, int height = 0, HMENU hMenu = NULL, LPVOID param = NULL){
		XWnd::createWindowEx(xApp->className(), windowName, style | WS_VISIBLE,
			parent ? parent->hWnd() : NULL, exStyle, x, y, width, height, hMenu, param);
	}
};

#endif // _X_WINDOW_HPP
