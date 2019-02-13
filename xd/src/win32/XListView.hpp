#ifndef _X_LIST_VIE_HPP
#define _X_LIST_VIE_HPP

#include "XWidget.hpp"

class XListView :public XWidget{
public:
	enum ViewMode{ListMode,IconMode,ReportMode,TileMode,SmallIconMode};
	XListView(XWidget*parent = nullptr) :XWidget(parent){
		XWidget::createWindowEx(WC_LISTVIEW, "", 0, parent);
	}
};

#endif // _X_LIST_VIE_HPP
