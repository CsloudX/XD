#ifndef _X_PAINTER_HPP
#define _X_PAINTER_HPP

#include "XApplication.hpp"
#include "XIcon.hpp"
#include "../XLine.hpp"
#include "../XColor.hpp"

class XPainter{
public:
	XPainter(XWnd* wnd) :_wnd(wnd)
	{
		assert(_wnd);
		//::InvalidateRect(_wnd->hWnd(),NULL , TRUE);
		_hdc = BeginPaint(_wnd->hWnd(), &_ps);
		//::SelectObject(_hdc, GetStockObject(SYSTEM_FIXED_FONT));
		//::SetBkMode(_hdc, TRANSPARENT);
	}
	~XPainter(){
		EndPaint(_wnd->hWnd(), &_ps);
		//::InvalidateRect(_wnd->hWnd(), NULL, TRUE);
	}

	HDC hDC()const{ return _hdc; }

	void setBgColor(int r, int g, int b){ SetBkColor(_hdc, RGB(r, g, b)); }
	void setBgColor(const XColor& color){ setBgColor(color.red(), color.green(), color.blue()); }
	void setFgColor(int r, int g, int b){ SetTextColor(_hdc, RGB(r, g, b)); }
	void setFgColor(const XColor& color){ setFgColor(color.red(), color.green(), color.blue()); }

	void drawText(const XPoint& pos, const XString& text){
		drawText(pos.x(), pos.y(), text);
	}
	void drawText(int x, int y, const XString& text){ ::TextOut(_hdc,x, y, text.c_str(), text.length()); }
	void drawIcon(const XPoint& pos, const XIcon& icon){ drawIcon(pos.x(), pos.y(), icon); }
	void drawIcon(int x, int y, const XIcon& icon){ ::DrawIcon(_hdc, x, y, icon.hIcon()); }
	void drawLine(int x1,int y1,int x2,int y2){
		::MoveToEx(_hdc, x1, y1,NULL);
		::LineTo(_hdc, x2, y2);
	}
	void drawLine(const XLine& line){ drawLine(line.pt1().x(), line.pt1().y(), line.pt2().x(), line.pt2().y()); }
	void drawRect(int left,int top,int right,int bottom){
		::Rectangle(_hdc, left, top, right, bottom);
	}
	void drawRect(const XRect& rect){ drawRect(rect.left(), rect.top(), rect.right(), rect.bottom()); }

private:
	XWnd*		_wnd;
	HDC			_hdc;
	PAINTSTRUCT  _ps;
};

#endif //_X_PAINTER_HPP
