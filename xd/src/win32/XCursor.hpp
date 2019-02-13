#ifndef _X_CURSOR_HPP
#define _X_CURSOR_HPP

class XCursor{
public:
	XCursor(HCURSOR hCursor) :_hCursor(hCursor){}
	HCURSOR hCursor()const{ return _hCursor; }
private:
	HCURSOR	_hCursor;
};


#endif //_X_CURSOR_HPP
