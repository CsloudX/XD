#ifndef _X_HBOX_LAYOUT_HPP
#define _X_HBOX_LAYOUT_HPP

#include "XBoxLayout.hpp"

class XHBoxLayout :public XBoxLayout{
public:
	XHBoxLayout(XWidget* parent = nullptr) :XBoxLayout(parent){}
	void addWidget(XWidget* widget, int stretch = 1){
		setStretch(_colCount, stretch);
		XGridLayout::addWidget(widget, 0, _colCount);
	}
	int spacing()const{ return XGridLayout::horizontalSpacing(); }
	void setSpacing(int spacing){ XGridLayout::setHorizontalSpacing(spacing); }
	int stretch(int index)const{ return XGridLayout::horizontalStretch(index); }
	void setStretch(int index, int stretch){ XGridLayout::setHorizontalStretch(index, stretch); }
};

#endif //_X_HBOX_LAYOUT_HPP
