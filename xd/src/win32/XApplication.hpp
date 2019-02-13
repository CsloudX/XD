#ifndef _X_APPLICATION_HPP
#define _X_APPLICATION_HPP

#include <unordered_map>
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment( lib, "comctl32.lib" )
#include "../XGlobal.h"
#include "../XString.hpp"
#include "../XRect.hpp"
#include "../XObject.hpp"
#include "XDbgHelper.hpp"

#define xApp XApplication::instance()
#define X_DESTROY_WINDOW(hWnd)	if(hWnd){::DestroyWindow(hWnd);hWnd=nullptr;}

class XWnd;

class XApplication{
public:
	friend class XWnd;

	XApplication()
	{
		assert(xApp == nullptr);
		*pInstance() = this;

		_className = L"XD";
		if (::FindWindow(_className.c_str(), NULL)){
			return;
		}

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = XApplication::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = NULL;// _hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = _className.c_str();
		wcex.hIconSm = NULL;
		RegisterClassEx(&wcex);
		_hInstance = ::GetModuleHandle(NULL);

		InitCommonControls();
	}
	~XApplication(){
		assert(xApp == this);
		*pInstance() = nullptr;
	}

	int exec(){
		//assert(wnds().size() > 0);
		MSG msg; 
		HACCEL hAccelTable = NULL;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			//xPrint(XString("%1 - %2 - %3\n").arg(msg.message).arg(msg.wParam).arg(msg.lParam));
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return (int)msg.wParam;
	}
	void processEvents()
	{
		MSG msg;
		HACCEL hAccelTable = NULL;
		if (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	void quit(){ PostQuitMessage(0); }

	const XString& className()const{ return _className; }
	HINSTANCE hInstance()const{ return _hInstance; }

	static XApplication* instance(){
		return *pInstance();
	}

private:
	XWnd* wnd(HWND hWnd)const{
		auto itr = wnds().find(hWnd);
		if (itr != wnds().end())
			return itr->second;
		return nullptr;
	}
	static void registerWnd(XWnd* wnd);
	static void unregisterWnd(XWnd* wnd);

	static XApplication** pInstance(){
		static XApplication* obj = nullptr;
		return &obj;
	}

	static std::unordered_map<HWND, XWnd*>& wnds(){
		static std::unordered_map<HWND, XWnd*> obj;
		return obj;
	}
	
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	
private:
	HINSTANCE	_hInstance;
	XString		_className;
};

class XWnd:public XObject{
public:
	friend class XApplication;
	XWnd(XWnd* parent = nullptr) :XObject(parent), _hWnd(nullptr), _lpPreWndProc(nullptr){}
	virtual ~XWnd(){
		xApp->unregisterWnd(this);
		X_DESTROY_WINDOW(_hWnd);
	}
	HWND hWnd()const{ return _hWnd; }
	void update(){ ::UpdateWindow(_hWnd); }

protected:
	void createWindowEx(const XString& className, const XString& windowName, DWORD style, HWND hParentWnd = NULL,
		DWORD exStyle=0,int x=0,int y=0,int width=0,int height=0,HMENU hMenu=NULL, LPVOID param=NULL){
		//if (hParentWnd)	style |= WS_CHILD;
		//else			style &= ~WS_CHILD;
		assert(_hWnd == nullptr);
		_hWnd = CreateWindowExW(exStyle, className.c_str(), windowName.c_str(), style,
			x, y, width, height, hParentWnd, hMenu, xApp->hInstance(), param);
		assert(_hWnd);
		xApp->registerWnd(this);


		if (hParentWnd){
			HANDLE hFont = (HANDLE)SendMessage(hParentWnd, WM_GETFONT, 0, 0);
			if (hFont==NULL)
				hFont = (HANDLE)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(_hWnd, WM_SETFONT, (WPARAM)hFont, 0);
		}

		if (className != xApp->className())
			_lpPreWndProc = (WNDPROC)SetWindowLong(_hWnd, GWL_WNDPROC,(LONG)XApplication::WndProc);
	}

	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam){
		if (_lpPreWndProc)
			return CallWindowProc(_lpPreWndProc, _hWnd, message, wParam, lParam);
		return DefWindowProc(_hWnd, message, wParam, lParam);
	}

	virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (_hWnd == hWnd){
			return WndProc(message, wParam, lParam);
		}

		if (_lpPreWndProc)
			return CallWindowProc(_lpPreWndProc, hWnd, message, wParam, lParam);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


protected:
	HWND	_hWnd;
	WNDPROC _lpPreWndProc;
};

inline void XApplication::registerWnd(XWnd* wnd)
{
	assert(wnd);
	wnds()[wnd->hWnd()] = wnd;
}
inline void XApplication::unregisterWnd(XWnd* wnd)
{
	auto itr = wnds().find(wnd->hWnd());
	if (itr != wnds().end()){
		wnds().erase(itr);
	}
}

inline LRESULT CALLBACK XApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto wnd = xApp->wnd(hWnd);
	if (wnd){
		return wnd->WndProc(message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

#endif // _X_APPLICATION_HPP