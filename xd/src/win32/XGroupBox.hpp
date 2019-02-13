#ifndef _X_GROUPBOX_HPP
#define _X_GROUPBOX_HPP

#include "XWidget.hpp"

class XGroupBox : public XWidget{
public:
	XGroupBox(const XString& text, XWidget* parent = nullptr):XWidget(parent){
		XWidget::createWindowEx("BUTTON", text, BS_GROUPBOX, parent);
	}
};

#endif //_X_GROUPBOX_HPP
