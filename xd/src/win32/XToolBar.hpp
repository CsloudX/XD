#ifndef _X_TOOL_BAR_HPP
#define _X_TOOL_BAR_HPP

#include "XApplication.hpp"

class XToolBar : public XWnd{
public:
	XToolBar(XWnd* parent) :XWnd(parent){
		assert(parent);

		createWindowEx(TOOLBARCLASSNAME, "", WS_CHILD | WS_VISIBLE , parent->hWnd());

		_tbButton.fsState = TBSTATE_ENABLED;
		_tbButton.fsStyle = TBSTYLE_BUTTON;
		_tbAddBitmap.hInst = HINST_COMMCTRL;
		_tbAddBitmap.nID = IDB_STD_SMALL_COLOR;
	}

	// id = btn_id, bitmap = STD_XXX (STD_FILENEW)
	void addButton(int id, int bitmap){
		_tbButton.idCommand = id;
		_tbButton.iBitmap = bitmap;
		SendMessage(_hWnd, TB_ADDBITMAP, 0, (LPARAM)&_tbAddBitmap);
		SendMessage(_hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(_hWnd, TB_ADDBUTTONS, 1, (LPARAM)&_tbButton);
		SendMessage(_hWnd, TB_AUTOSIZE, 0, 0);
	}

	void update(){ SendMessage(_hWnd, TB_AUTOSIZE, 0, 0); }

private:
	TBBUTTON	_tbButton;
	TBADDBITMAP _tbAddBitmap;
};

#endif // _X_TOOL_BAR_HPP