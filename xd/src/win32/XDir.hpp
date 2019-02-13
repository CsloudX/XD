#ifndef _X_DIR_HPP
#define _X_DIR_HPP

#include "../XString.hpp"
#include "XFileInfo.hpp"

class XDir{
public:
	enum Filter{
		NoFilter = 0,
		Dirs = 1,
		Files = 2,
		NotDot = 4,
		NotDotDot = 8
	};
	XDir(const XString& path):_path(path){
		if (_path.endWith("/"))
			_path = _path.left(_path.length() - 1);
	}

	XFileInfoList entryInfoList(int filter = NoFilter,const XString& nameFilter="*"){
		XFileInfoList fileInfos;
		WIN32_FIND_DATA	wfd;
		HANDLE hFind = FindFirstFile(XString("%1/%2").arg(_path).arg(nameFilter).c_str(), &wfd);
		if (hFind == INVALID_HANDLE_VALUE){
			return fileInfos;
		}

		if (filter == NoFilter){
			do{
				fileInfos.append(XFileInfo(XString("%1/%2").arg(_path).arg(wfd.cFileName)));
			} while (FindNextFile(hFind, &wfd) != 0);
		}
		else{
			do{
				if ((filter&XDir::Dirs) && !(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					continue;
				if ((filter&XDir::Files) && (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					continue;
				if ((filter&XDir::NotDot) && (wfd.cFileName == XString(".")))
					continue;
				if ((filter&XDir::NotDotDot) && (wfd.cFileName == XString("..")))
					continue;

				fileInfos.append(XFileInfo(XString("%1/%2").arg(_path).arg(wfd.cFileName)));
			} while (FindNextFile(hFind, &wfd) != 0);
		}

		FindClose(hFind);
		return fileInfos;
	}

	XStringList entryList(int filter = NoFilter,const XString& nameFilter="*")
	{
		XStringList files;
		auto fileInfos = entryInfoList(filter,nameFilter);
		for (auto& var : fileInfos){
			files.append(var.filePath());
		}
		return files;
	}

private:
	XString	_path;
};

#endif //_X_DIR_HPP
