#ifndef _X_PUSHBUTTON_HPP
#define _X_PUSHBUTTON_HPP

#include "XAbstractButton.hpp"

class XPushButton:public XAbstractButton{
public:
	XPushButton(const XString& text, XWidget* parent = nullptr, int id=0) :XAbstractButton(parent){
		XAbstractButton::createWindowEx(text, BS_PUSHBUTTON, id,parent);
	}

	bool isDefault()const{ return  ::GetWindowLong(_hWnd, GWL_STYLE)&BS_DEFPUSHBUTTON; }
	void setDefault(bool b){
		DWORD style = ::GetWindowLong(_hWnd, GWL_STYLE);
		if (b)
			style |= BS_DEFPUSHBUTTON;
		else
			style &= (~BS_DEFPUSHBUTTON);
		::SetWindowLong(_hWnd, GWL_STYLE, style);
	}

private:
};

#endif //_X_PUSHBUTTON_HPP
