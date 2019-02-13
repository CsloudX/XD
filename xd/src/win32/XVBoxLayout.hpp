#ifndef _X_VBOX_LAYOUT_HPP
#define _X_VBOX_LAYOUT_HPP

#include "XBoxLayout.hpp"

class XVBoxLayout :public XBoxLayout{
public:
	XVBoxLayout(XWidget* parent = nullptr) :XBoxLayout(parent){}
	void addWidget(XWidget* widget, int stretch = 1){
		setStretch(_rowCount, stretch);
		XGridLayout::addWidget(widget, _rowCount, 0);
	}
	int spacing()const{ return XGridLayout::verticalSpacing(); }
	void setSpacing(int spacing){ XGridLayout::setVerticalSpacing(spacing); }
	int stretch(int index)const{ return XGridLayout::verticalStretch(index); }
	void setStretch(int index, int stretch){ XGridLayout::setVerticalStretch(index, stretch); }
};

#endif //_X_VBOX_LAYOUT_HPP
