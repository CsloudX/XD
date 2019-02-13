#ifndef _X_RADIO_BUTTON_HPP
#define _X_RADIO_BUTTON_HPP

#include "XAbstractButton.hpp"

class XRadioButton : public XAbstractButton{
public:
	XRadioButton(const XString& text, XWidget* parent = nullptr){
		createButton(text, BS_AUTORADIOBUTTON, parent);
	}

};


#endif //_X_RADIO_BUTTON_HPP
