#ifndef _X_ICON_HPP
#define _X_ICON_HPP

class XIcon{
public:
	XIcon(HICON hIcon=nullptr) :_hIcon(hIcon){}
	~XIcon(){
		destroy();
	}
	HICON hIcon()const{ return _hIcon; }
	void load(HINSTANCE hInstance,LPCWSTR lpIconName){
		destroy();
		_hIcon = ::LoadIconW(hInstance, lpIconName);
	}
private:
	void destroy(){
		if (_hIcon){
			::DestroyIcon(_hIcon);
		}
	}
private:
	HICON	_hIcon;
};

#endif //_X_ICON_HPP
