#ifndef _X_REG_KEY_HPP
#define _X_REG_KEY_HPP

#include <cassert>

class XRegKey{
public:
	// hKey = HKEY_XXX_YYY
	XRegKey(HKEY hKey, const XString& path){ open(hKey, path); }
	XRegKey(const XRegKey& regKey, const XString& path){ open(regKey._hKey, path); }
	~XRegKey(){ close(); }

	// type=REG_NONE,REG_SZ ...
	void setValue(const XString& name, DWORD type, const BYTE* data, DWORD size){
		RegSetValueEx(_hKey, name.c_str(), 0, type, data, size);
	}
	bool queryValue(const XString& name, LPDWORD type, LPBYTE data, LPDWORD size){
		return RegQueryValueEx(_hKey, name.c_str(), 0, type, data, size)==0;
	}
	void setValue(const XString& name, const XString& value){
		RegSetValueEx(_hKey, name.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), sizeof(wchar_t)*value.length());
	}
	void removeValue(const XString& name){ RegDeleteValue(_hKey, name.c_str()); }
	XString stringValue(const XString& name=XString()){
		DWORD type = 0;
		wchar_t* data = nullptr;
		DWORD size = 0;
		RegQueryValueEx(_hKey, name.c_str(), 0, &type, (LPBYTE)data, &size);
		if (type == REG_SZ){
			data = new wchar_t[size / sizeof(wchar_t)];
			RegQueryValueEx(_hKey, name.c_str(), 0, &type, (LPBYTE)data, &size);
			XString str(data);
			delete[] data;
			return str;
		}
		return XString();
	}


	XStringList keys(){
		XStringList ks;
		int idx = 0;
		TCHAR key[MAX_PATH];
		DWORD len = MAX_PATH;
		while (RegEnumKeyEx(_hKey, idx++, key, &len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS){
			ks.append(XString(key, len));
			len = MAX_PATH;
		}
		return ks;
	}
	bool getKey(int idx, XString& key){
		TCHAR buf[MAX_PATH];
		DWORD len = MAX_PATH;
		if (RegEnumKeyEx(_hKey, idx, buf, &len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS){
			key = XString(buf, len);
			return true;
		}
		return false;
	}
	XString key(int idx){
		XString retval;
		getKey(idx, retval);
		return retval;
	}

	XStringList valueNames(){
		XStringList vs;
		int idx = 0;
		TCHAR val[MAX_PATH];
		DWORD len = MAX_PATH;
		while (RegEnumValue(_hKey,idx++,val,&len,NULL,NULL,NULL,NULL) == ERROR_SUCCESS){
			vs.append(XString(val, len));
			len = MAX_PATH;
		}
		return vs;
	}

private:
	bool open(HKEY hKey, const XString& path){
		_path = path;
		return (ERROR_SUCCESS==RegCreateKey(hKey, path.c_str(), &_hKey));
	}
	void close() 
	{ 
		if(_hKey)
			RegCloseKey(_hKey);
		_hKey = NULL;
	}

	

private:
	HKEY	_hKey;
	XString _path;
};

#endif //_X_REG_KEY_HPP
