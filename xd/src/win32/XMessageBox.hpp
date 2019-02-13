#ifndef _X_MESSAGEBOX_HPP
#define _X_MESSAGEBOX_HPP

#include "XWidget.hpp"

class XMessageBox{
public:
	static void information();
	static void question();
	static void warning(XWidget* parent, const XString& title, const XString& text){
		messageBox(parent, title, text, MB_OK);
	}
	static void critical();

private:
	static int messageBox(XWidget* parent, const XString& title, const XString& text,UINT uType){
		return ::MessageBox(parent ? parent->hWnd() : NULL, text.c_str(), title.c_str(), uType);
	}
};

#endif //_X_MESSAGEBOX_HPP
