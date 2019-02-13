#ifndef _X_FRAME_HPP
#define _X_FRAME_HPP

#include "XWidget.hpp"
#include "XPainter.hpp"

class XFrame : public XWidget{
public:
	XFrame(XWidget* parent = nullptr):XWidget(parent){
		//XWidget::createWindowEx("STATIC", "", 0, parent);
		XWidget::createWindowEx(xApp->className(), "", 0, parent);
	}
	void setBlackFrame(bool b){ changeStyle(b ,SS_BLACKFRAME); }
	void setGrayFrame(bool b){ changeStyle(b, SS_GRAYFRAME); }
	void setBlackRect(bool b){ changeStyle(b, SS_BLACKRECT); }
	void setGrayRect(bool b){ changeStyle(b, SS_GRAYRECT); }
	void setEtchedFrame(bool b){ changeStyle(b, SS_ETCHEDFRAME); }
	void setSunken(bool b){changeExStyle(b, SS_SUNKEN | WS_EX_STATICEDGE);}

protected:
	XSize layoutSizeHint(){
		RECT rt;
		::GetClientRect(_hWnd, &rt);
		return XSize(rt.right - rt.left, rt.bottom - rt.top);
	}
};

#endif //_X_FRAME_HPP
