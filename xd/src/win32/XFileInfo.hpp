#ifndef _X_FILE_INFO_HPP
#define _X_FILE_INFO_HPP

#include "../XString.hpp"

class XFileInfo{
public:
	XFileInfo(const XString& path) :_path(path){}
	XString fileName()const;
	XString filePath()const{ return _path; }
private:
	XString _path;
};

typedef XList<XFileInfo> XFileInfoList;

#endif //_X_FILE_INFO_HPP
