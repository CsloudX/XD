#ifndef _X_LIST_BOX_HPP
#define _X_LIST_BOX_HPP

#include "XWidget.hpp"



class XListBox :public XWidget{
public:
	enum SelectionMode{SingleSelection,MultiSelection};
	XListBox(XWidget*parent = nullptr,int id = 0) :XWidget(parent){
		XWidget::createWindowEx(WC_LISTBOX, "", LBS_NOTIFY | WS_VSCROLL | WS_BORDER, parent, 0, 0, 0, 0, 0, (HMENU)id);
	}

	void setRedrawEnable(bool b){ SendMessage(_hWnd, WM_SETREDRAW, b ? TRUE : FALSE, 0); }

	void addItem(const XString& text){ SendMessage(_hWnd, LB_ADDSTRING, 0, (LPARAM)text.c_str()); }
	void addItems(const XStringList& texts){
		setRedrawEnable(false);
		for (auto& var : texts){
			addItem(var);
		}
		setRedrawEnable(true);
	}
	void insertItem(int idx, const XString& text){ SendMessage(_hWnd, LB_INSERTSTRING, idx, (LPARAM)text.c_str()); }
	void removeItem(int idx){ SendMessage(_hWnd, LB_DELETESTRING, idx, 0); }
	void clear(){ SendMessage(_hWnd, LB_RESETCONTENT, 0, 0); }
	int count()const{ return SendMessage(_hWnd, LB_GETCOUNT, 0, 0); }
	void setCurrentIndex(int idx){ SendMessage(_hWnd, LB_SETCURSEL, idx, 0); }
	int currentIndex()const{ return SendMessage(_hWnd, LB_GETCURSEL, 0, 0); }
	XString text(int idx){
		int len = SendMessage(_hWnd, LB_GETTEXTLEN, idx, 0);
		wchar_t* buf = new wchar_t[len+1];
		SendMessage(_hWnd, LB_GETTEXT, idx, (LPARAM)buf);
		XString text(buf,len);
		delete[] buf;
		return text;
	}
	//void setSelectionMode(SelectionMode mode){
	//	changeStyle(mode==MultiSelection, LBS_MULTIPLESEL);
	//}
	void setRowHeight(int height){
		SendMessage(_hWnd, LB_SETITEMHEIGHT, 0, height);
	}
};

#endif //_X_LIST_BOX_HPP
