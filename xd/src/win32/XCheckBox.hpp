#ifndef _X_CHECK_BOX_HPP
#define _X_CHECK_BOX_HPP

#include "XAbstractButton.hpp"

class XCheckBox : public XAbstractButton{
public:
	XCheckBox(const XString& text, XWidget* parent = nullptr){
		createButton(text, BS_AUTOCHECKBOX, parent);
	}
	void setTristate(bool b = true){
		DWORD style = ::GetWindowLong(_hWnd, GWL_STYLE);
		if (b)
			style |= BS_AUTO3STATE;
		else
			style &= (~BS_AUTO3STATE);
		style &= (~0x01);
		::SetWindowLong(_hWnd, GWL_STYLE, style);
	}
};

#endif //_X_CHECK_BOX_HPP
