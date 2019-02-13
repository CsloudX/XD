#ifndef _X_LINE_EDIT_HPP
#define _X_LINE_EDIT_HPP

#include "XWidget.hpp"

class XLineEdit :public XWidget{
public:
	enum EchoMode{Normal,Password};
	XLineEdit(XWidget* parent = nullptr):XWidget(parent){
		XWidget::createWindowEx("EDIT", "",  ES_AUTOHSCROLL , parent, WS_EX_CLIENTEDGE);
		//XWidget::createWindowEx("EDIT", "", 0, parent, 50010080);
	}
	void setText(const XString& text){ XWidget::setWindowText(text); }
	void setClientEdge(bool b){ XWidget::setClientEdge(b); }
	void setAlignment(int align){
		assert(::IsWindow(_hWnd));
		auto style = ::GetWindowLong(_hWnd, GWL_STYLE);
		style &= ~(SS_LEFT | SS_CENTER | SS_RIGHT | SS_CENTERIMAGE);
		if (align & Xd::AlignLeft)		style |= ES_LEFT;
		if (align & Xd::AlignHCenter)	style |= ES_CENTER;
		if (align & Xd::AlignRight)		style |= ES_RIGHT;
		::SetWindowLong(_hWnd, GWL_STYLE, style);
		::InvalidateRect(_hWnd, NULL, FALSE);
	}
	//void setEchoMode(EchoMode mode){ changeStyle(mode == Password, ES_PASSWORD); }
	//void setReadOnly(bool b){ changeExStyle(b, ES_READONLY); }
};

#endif //_X_LINE_EDIT_HPP