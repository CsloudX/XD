#ifndef _X_COMBO_BOX_HPP
#define _X_COMBO_BOX_HPP

#include "XWidget.hpp"

class XComboBox :public XWidget{
public:
	XComboBox(XWidget* parent = nullptr) :XWidget(parent){
		XWidget::createWindowEx(WC_COMBOBOX,"",0,parent);
	}
};

#endif //_X_COMBO_BOX_HPP
