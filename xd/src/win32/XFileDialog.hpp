#ifndef _X_FILE_DIALOG_HPP
#define _X_FILE_DIALOG_HPP

#include <commdlg.h>
#include "XDialog.hpp"

class XFileDialog{
public:
	static XString getOpenFileName(XWidget* parent = nullptr, 
		const XString& title = XString(),const XString& dir=XString()){

		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(ofn));
		TCHAR szFilePath[MAX_PATH];
		memset(szFilePath, 0, MAX_PATH);

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = parent?parent->hWnd():NULL;
		ofn.hInstance = ::GetModuleHandle(NULL);
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrInitialDir = dir.c_str();
		ofn.lpstrFile = szFilePath;
		ofn.lpstrTitle = title.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

		if (::GetOpenFileName(&ofn))
			return XString(szFilePath);
		return XString();
	}
};

#endif //_X_FILE_DIALOG_HPP
