#ifndef _X_STATUS_BAR_HPP
#define _X_STATUS_BAR_HPP

#include "XApplication.hpp"

class XStatusBar : public XWnd{
public:
	XStatusBar(XWnd* parent) :XWnd(parent){
		assert(parent);
		createWindowEx(STATUSCLASSNAME, "", WS_CHILD | WS_VISIBLE , parent->hWnd());
	}

	void update(){ MoveWindow(_hWnd, 0, 0, 0, 0, TRUE); }

	void showMessage(const XString& text){
		SendMessage(_hWnd, SB_SETTEXT, 0, (LPARAM)(text.c_str()));
	}

private:
};

#endif //_X_STATUS_BAR_HPP
