#ifndef _X_FONT_HPP
#define _X_FONT_HPP

class XFont{
public:
	XFont(HFONT hFont = nullptr) :_hFont(hFont){}
	~XFont(){}

	HFONT hFont()const{ return _hFont; }

private:
	HFONT _hFont;
};

#endif //_X_FONT_HPP
