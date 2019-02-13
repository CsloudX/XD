#ifndef _X_ABSTRACT_BUTTON_HPP
#define _X_ABSTRACT_BUTTON_HPP

#include "XWidget.hpp"

class XAbstractButton:public XWidget{
public:
	XAbstractButton(XWidget* parent = nullptr) :XWidget(parent){}
	void setText(const XString& text){ XWidget::setWindowText(text); }
	//void setAlignment(int align)
protected:
	void createWindowEx(const XString& text, DWORD style, DWORD id, XWidget* parent){
		XWidget::createWindowEx("BUTTON", text, style, parent, 0, 0, 0, 0, 0,(HMENU)id);
	}
};

#endif //_X_ABSTRACT_BUTTON_HPP
