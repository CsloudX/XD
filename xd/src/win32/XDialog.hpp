#ifndef _X_DIALOG_HPP
#define _X_DIALOG_HPP

#include "XWindow.hpp"

class XDialog :public XWidget{
public:
	enum DialogCode{ Rejected, Accepted };
	XDialog(const XString& title=XString(),XWidget* parent = nullptr) :XWidget(parent),_exitCode(0){
		XWnd::createWindowEx(xApp->className(), title,
			WS_OVERLAPPEDWINDOW,
			parent ? parent->hWnd() : NULL);

		if (parent){
			moveParentCenter();
		}
	}

	int exec(){
		setVisible(true);
		while (isVisible()){
			MSG msg;
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return _exitCode;
	}
	void done(int code){ _exitCode = code; hidden(); }
	void accept(){ done(Accepted); }
	void reject(){ done(Rejected); }

protected:
	XSize layoutSizeHint(){
		RECT rt;
		::GetClientRect(_hWnd, &rt);
		return XSize(rt.right - rt.left, rt.bottom - rt.top);
	}

	void showEvent(XShowEvent* event){
		if (parent()){
			parent()->setEnabled(false);
		}
	}

	void hideEvent(XHideEvent* event){
		if (parent()){
			parent()->setEnabled(true);
		}
	}

	bool nativeEvent(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
		{
			XCloseEvent event;
			closeEvent(&event);
			hidden();
		}
			return true;
		}
		return false;
	}

private:
	int		_exitCode;
};


#endif //_X_DIALOG_HPP
