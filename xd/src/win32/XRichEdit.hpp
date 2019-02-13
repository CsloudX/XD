#ifndef _X_RICH_EDIT_HPP
#define _X_RICH_EDIT_HPP

#include <Richedit.h>
#include "XWidget.hpp"

class XRichEdit : public XWidget
{
public:
	XRichEdit(XWidget* parent = nullptr) :XWidget(parent)
	{
		LoadLibrary(L"msftedit.dll");
		XWidget::createWindowEx(MSFTEDIT_CLASS, "", 
			WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | WS_VSCROLL,
			parent);
	}
};

#endif //_X_RICH_EDIT_HPP

