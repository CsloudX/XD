#ifndef _X_LABEL_HPP
#define _X_LABEL_HPP

#include "XWidget.hpp"
#include "../XNamespace.h"

class XLabel :public XWidget{
public:
	XLabel(const XString& text, XWidget* parent = nullptr)
		:XWidget(parent){
		XWidget::createWindowEx("STATIC", text, 0, parent);
		//setText(text);
	}
	void setText(const XString& text){ XWidget::setWindowText(text); }
	void setAlignment(int align){

		assert(::IsWindow(_hWnd));
		auto style = ::GetWindowLong(_hWnd, GWL_STYLE);
		style &= ~(SS_LEFT | SS_CENTER | SS_RIGHT | SS_CENTERIMAGE);
		if (align & Xd::AlignLeft)		style |= SS_LEFT;
		if (align & Xd::AlignHCenter)	style |= SS_CENTER;
		if (align & Xd::AlignRight)		style |= SS_RIGHT;
		if (align & Xd::AlignVCenter)	style |= SS_CENTERIMAGE;
		::SetWindowLong(_hWnd, GWL_STYLE, style);
		::InvalidateRect(_hWnd, NULL, FALSE);
	}

	void setBlackFrame(bool b){ changeStyle(b, SS_BLACKFRAME); }
	void setGrayFrame(bool b){ changeStyle(b, SS_GRAYFRAME); }
	void setBlackRect(bool b){ changeStyle(b, SS_BLACKRECT); }
	void setGrayRect(bool b){ changeStyle(b, SS_GRAYRECT); }
	void setEtchedFrame(bool b){ changeStyle(b, SS_ETCHEDFRAME); }
	void setSunken(bool b){ changeExStyle(b, SS_SUNKEN | WS_EX_STATICEDGE); }

};

#endif //_X_LABEL_HPP
