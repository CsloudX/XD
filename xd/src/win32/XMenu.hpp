#ifndef _X_MENU_HPP
#define _X_MENU_HPP

#include "XObject.hpp"

class XMenu:public XObject{
public:
	XMenu(XObject* parent = nullptr) :XObject(parent){
		_hMenu = CreateMenu();
	}
	//XMenu(HMENU hMenu = nullptr):_hMenu(hMenu){}
	~XMenu(){
		destroy();
	}
	void addMenu(const XString& text, int id){
		AppendMenu(_hMenu, MF_STRING, id, text.c_str());
	}
	void addSeparator(){
		AppendMenu(_hMenu, MF_SEPARATOR, 0, NULL);
	}
	//void load(HINSTANCE hInstance,LPCWSTR lpMenuName){
	//	destroy();
	//	_hMenu = ::LoadMenu(hInstance, lpMenuName);
	//}
	HMENU hMenu()const{ return _hMenu; }
	void setCheckable(bool b);
	void setEnabled(bool b);
	void setGray(bool b);
private:
	void destroy(){
		if (_hMenu){
			::DestroyMenu(_hMenu);
		}
	}
private:
	HMENU _hMenu;
};

class XMenuBar :public XObject{
public :
	XMenuBar(XObject* parent = nullptr) :XObject(parent){
		_hMenu = CreateMenu();
	}
	~XMenuBar(){
		if (_hMenu){
			DestroyMenu(_hMenu);
		}
	}
	HMENU hMenu()const{ return _hMenu; }
	XMenu* addMenu(const XString& text){
		XMenu* menu = new XMenu(this);
		AppendMenu(_hMenu, MF_POPUP, (UINT_PTR)menu->hMenu(), text.c_str());
		return menu;
	}

private:
	HMENU _hMenu;
};

#endif //_X_MENU_HPP
