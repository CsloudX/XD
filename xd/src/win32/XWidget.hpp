#ifndef _X_WIDGET_HPP
#define _X_WIDGET_HPP

#include "XApplication.hpp"
#include "XIcon.hpp"
#include "XCursor.hpp"
#include "XLayout.hpp"
#include "XFont.hpp"
#include "../XRect.hpp"

class XPaintEvent;

class XEvent{

};

class XButtonClickEvent:public XEvent{
public:
	XButtonClickEvent(HWND hWnd,DWORD id) :_hWnd(hWnd),_id(id){}
	HWND hWnd()const{ return _hWnd; }
	DWORD id()const{ return _id; }
private:
	HWND	_hWnd;
	DWORD	_id;
};
class XMenuEvent:public XEvent{
public:
	XMenuEvent(HMENU hMenu,DWORD id=0) :_hMenu(hMenu),_id(id){}
	HMENU hMenu()const{ return _hMenu; }
	DWORD id()const { return _id; }
private:
	HMENU	_hMenu;
	DWORD	_id;
};

class XCommandEvent :public XEvent{
public:
	XCommandEvent(DWORD id) :_id(id){}
	DWORD id()const{ return _id; }
private:
	DWORD _id;
};

class XResizeEvent:public XEvent{
public:
	XResizeEvent(const XSize& size, const XSize& oldSize)
	:_size(size),_oldSize(oldSize){
	}
	const XSize& size()const{ return _size; }
	const XSize& oldSize()const{ return _oldSize; }

private:
	XSize	_size;
	XSize	_oldSize;
};

class XHideEvent :public XEvent{

};
class XShowEvent :public XEvent{

};
class XCloseEvent :public XEvent{};
class XListBoxSelChangeEvent :public XEvent{
public:
	XListBoxSelChangeEvent(int id) :_id(id){}
	int id()const{ return _id; }
private:
	int _id;
};

class XWidget:public XWnd{
public:
	XWidget(XWidget*parent = nullptr) 
		:XWnd(parent), _layout(nullptr)
		,m_fixedSizeFlag(false),m_minSizeFlag(false),m_maxSizeFlag(false){}
	virtual ~XWidget(){}
	const XRect rect()const{
		RECT rt;
		::GetWindowRect(_hWnd, &rt);
		POINT pos{ rt.left, rt.top };
		HWND parent = ::GetParent(_hWnd);
		if (parent){
			::ScreenToClient(parent, &pos);
		}
		return XRect(pos.x, pos.y, rt.right - rt.left, rt.bottom - rt.top);
	}
	void setRect(int x, int y, int w, int h){
		assert(::IsWindow(_hWnd));
		::SetWindowPos(_hWnd, 0, x, y, w, h, SWP_NOZORDER );
		updateLayout();
	}
	void setRect(const XRect& rect){ setRect(rect.x(), rect.y(), rect.width(), rect.height()); }
	XSize size()const{ return rect().size(); }
	int width()const{ return size().width(); }
	int height()const{ return size().height(); }
	const XPoint& pos()const{ return rect().topLeft(); }
	void setFont(const XFont& font){ SendMessage(_hWnd, WM_SETFONT, (WPARAM)font.hFont(), TRUE); }
	const XFont& font()const;
	bool hasFocus()const;
	bool isEnabled()const;
	bool isFullScreen()const;
	bool isHidden()const;
	bool isMaximized()const;
	bool isMinimized()const;
	bool isModal()const;
	void setVisible(bool b){ 
		assert(::IsWindow(_hWnd));
		::ShowWindow(_hWnd, b ? SW_SHOW : SW_HIDE); 
	}
	void show(){ setVisible(true); }
	void hidden(){ setVisible(false); }
	bool isVisible()const{ return IsWindowVisible(_hWnd)==TRUE; }
	//void setBorder(bool b){ changeWindowStyle(b, WS_BORDER); }
	void setEnabled(bool b){ changeStyle(!b, WS_DISABLED); }
	const XSize& maximumSize()const;
	const XSize& minimumSize()const;
	void move(const XPoint& pos){ return move(pos.x(), pos.y()); }
	void move(int x, int y){
		assert(::IsWindow(_hWnd));
		::SetWindowPos(_hWnd, 0, x,y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	}
	void resize(const XSize& size){
		resize(size.width(), size.height());
	}
	void resize(int w, int h){ 
		assert(::IsWindow(_hWnd));
		::SetWindowPos(_hWnd, 0, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
		updateLayout();
	}
	//void setFixedSize(const XSize& size)
	//{
	//	m_fixedSizeFlag = true;
	//	m_fixedSize = size;
	//	resize(m_fixedSize);
	//	changeExStyle(false, WS_THICKFRAME);
	//}
	//void setFixedSize(int w, int h){ setFixedSize(XSize(w, h)); }

	void setCursor(const XCursor& cursor){
		::SetClassLong(_hWnd, GCL_HCURSOR, (LONG)(cursor.hCursor()));
	}


	void setLayout(XLayout* layout){ 
		if (layout == _layout)
			return;

		assert(_layout == nullptr);
		assert(layout);
		_layout = layout; 
		_layout->setParent(this);
	}

	void setWindowText(const XString& text){
		assert(::IsWindow(_hWnd));
		::SetWindowText(_hWnd, text.c_str());
	}

	XWidget* parent()const{ return static_cast<XWidget*>(_parent); }

	void moveParentCenter(){
		assert(_parent);
		auto rt = rect();
		rt.moveCenter(parent()->rect().center());
		setRect(rt);
	}

protected:
	void modifyWindowLong(int nOffset, DWORD dwAdd, DWORD dwRemove,UINT nFlags=0){
		assert(_hWnd);
		DWORD value = ::GetWindowLong(_hWnd, nOffset);
		DWORD newValue = (value & ~dwRemove) | dwAdd;
		if (value == newValue)
			return;
		::SetWindowLong(_hWnd, nOffset, newValue);
		if (nFlags != 0){
			::SetWindowPos(_hWnd, NULL, 0, 0, 0, 0,
				SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
		}
	}
	void modifyStyle(DWORD dwAdd, DWORD dwRemove,UINT nFlags=0){ modifyWindowLong(GWL_STYLE, dwAdd, dwRemove,nFlags); }
	void modifyExStyle(DWORD dwAdd, DWORD dwRemove,UINT nFlags=0){ modifyWindowLong(GWL_EXSTYLE, dwAdd, dwRemove,nFlags); }
	void changeStyle(bool b, DWORD style,UINT nFlags=0){
		if (b)	modifyStyle(style, 0,nFlags);
		else	modifyStyle(0, style,nFlags);
		//::InvalidateRect(_hWnd, NULL, FALSE);
	}
	void changeExStyle(bool b, DWORD style,UINT nFlags=0){
		if (b)	modifyExStyle(style, 0,nFlags);
		else	modifyExStyle(0, style,nFlags);
		//::InvalidateRect(_hWnd, NULL, FALSE);
	}
	void createWindowEx(const XString& className, const XString& windowName, DWORD style, XWidget* parent,
		DWORD exStyle = 0, int x = 0, int y = 0, int width = 0, int height = 0, HMENU hMenu = NULL, LPVOID param = NULL){

		if (parent){
			style |= (WS_VISIBLE|WS_CHILD);
		}
		else{
			style |= WS_OVERLAPPEDWINDOW;
			style &= ~WS_CHILD;
		}
		
		XWnd::createWindowEx(className, windowName, style, 
			parent ? parent->hWnd() : NULL, exStyle, x, y, width, height, hMenu, param);
	}

	virtual void paintEvent(XPaintEvent* event){ X_UNUSED(event); }
	virtual void buttonClickEvent(XButtonClickEvent* event){ if (_parent)	static_cast<XWidget*>(_parent)->buttonClickEvent(event); }
	virtual void menuInitEvent(XMenuEvent* event){ X_UNUSED(event); }
	virtual void menuSelectEvent(XMenuEvent* event){ X_UNUSED(event); }
	virtual void menuClickEvent(XMenuEvent* event){ X_UNUSED(event); }
	virtual void commandEvent(XCommandEvent* event){ X_UNUSED(event); }
	virtual void resizeEvent(XResizeEvent* event){ X_UNUSED(event); }
	virtual void hideEvent(XHideEvent* event){ X_UNUSED(event); }
	virtual void showEvent(XShowEvent* event){ X_UNUSED(event); }
	virtual void closeEvent(XCloseEvent*){}
	virtual void listBoxSelChangeEvent(XListBoxSelChangeEvent* event){ if (_parent)	static_cast<XWidget*>(_parent)->listBoxSelChangeEvent(event); }
	virtual bool nativeEvent(UINT message, WPARAM wParam, LPARAM lParam){
		X_UNUSED(message); X_UNUSED(wParam); X_UNUSED(lParam);
		return false;
	}

	virtual XSize layoutSizeHint(){ return size(); }
	void updateLayout(){
		if (_layout){
			_layout->update(layoutSizeHint());
		}
	}


	void setIcon(const XIcon& icon){
		assert(::IsWindow(_hWnd));
		::SetClassLong(_hWnd, GCL_HICONSM, (LONG)icon.hIcon());
	}

	void setClientEdge(bool b){ 
		changeExStyle(b, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
		update();
	}


	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		//xPrint(XString("%1 - %2 - %3\n").arg(message).arg(wParam).arg(lParam));
		if (nativeEvent(message, wParam, lParam))
			return 0;

		switch (message)
		{
		case WM_PAINT:
			paintEvent(nullptr);
			break;
		case WM_COMMAND:{
			auto id = LOWORD(wParam);
			auto code = HIWORD(wParam);
			auto handle = lParam;

			{XCommandEvent event(id); commandEvent(&event); }

			if (code == 0 && handle == 0){
				XMenuEvent event(0, id);
				menuClickEvent(&event);
			}
			else if(code == 0){
				XButtonClickEvent event((HWND)handle,id);
				buttonClickEvent(&event);
			}
			else
			{
				if (code ==LBN_SELCHANGE)
				{XListBoxSelChangeEvent event(id); listBoxSelChangeEvent(&event);}
			}
		}
			break;
		case WM_INITMENU:{
			XMenuEvent event((HMENU)wParam);
			menuInitEvent(&event);
		}
			break;
		case WM_MENUSELECT:
			menuSelectEvent(nullptr);
			break;
		case WM_SIZE:{
			XSize size(LOWORD(lParam), HIWORD(lParam));
			XResizeEvent event(size, this->size());
			resizeEvent(&event);
			updateLayout();
		}
			break;
		case WM_CLOSE:
		{
			XCloseEvent event;
			closeEvent(&event);
			return 0;
		}
		case WM_SHOWWINDOW:
			if (wParam == TRUE){
				XShowEvent event; showEvent(&event);
			}
			else if (wParam == FALSE){
				XHideEvent event; hideEvent(&event);
			}
			return 0;
		}
		return XWnd::WndProc(message, wParam, lParam);
	}

protected:
	XLayout*	_layout;
	bool	m_fixedSizeFlag;
	XSize	m_fixedSize;
	bool	m_minSizeFlag;
	XSize	m_minSize;
	bool	m_maxSizeFlag;
	XSize	m_maxSize;
};

#endif //_X_WIDGET_HPP
