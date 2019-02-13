#ifndef _X_BOX_LAYOUT_HPP
#define _X_BOX_LAYOUT_HPP


#include "XGridLayout.hpp"

class XBoxLayout :public XGridLayout{
public:
	XBoxLayout(XWidget* parent = nullptr) :XGridLayout(parent){}
	virtual ~XBoxLayout(){}
	virtual void addWidget(XWidget* widget, int stretch = 1) = 0;
	void addLayout(XLayout* layout, int stretch = 1){
		XWidget* parent = static_cast<XWidget*>(_parent);
		assert(parent);
		XFrame* frame = new XFrame(parent);
		frame->setLayout(layout);
		layout->setMargins(0);
		addWidget(frame,stretch);
	}
	virtual int spacing()const = 0;
	virtual void setSpacing(int spacing) = 0;
	virtual int stretch(int index)const = 0;
	virtual void setStretch(int index, int stretch) = 0;

private:
	void addWidget(XWidget* widget, int row, int col, int rowSpan = 1, int colSpan = 1);
	void setHorizontalSpacing(int spacing);
	void setVerticalSpacing(int spacing);
	int horizontalSpacing()const;
	int verticalSpacing()const;
	void setHorizontalStretch(int index, int stretch);
	void setVerticalStretch(int index, int stretch);
	int horizontalStretch(int index)const;
	int verticalStretch(int index)const;
};

#endif //_X_BOX_LAYOUT_HPP
