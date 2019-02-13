#ifndef _X_DBGHELP_HPP
#define _X_DBGHELP_HPP


#include <windows.h>
#include <cstdio>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")
#include "../XString.hpp"
#include "../XMutexLocker.hpp"

#pragma warning(disable:4996)
class XConsole
{
public:
	XConsole(){
		AllocConsole();
		freopen("conin$", "r+t", stdin);
		freopen("conout$", "w+t", stdout);
		freopen("conout$", "w+t", stderr);
	}
	~XConsole(){
		FreeConsole();
		fclose(stderr);
		fclose(stdout);
		fclose(stdin);
	}
};
#pragma warning(default:4996)

class XDbgHelper{
public:
	static void installDumpFilter()
	{
		::SetUnhandledExceptionFilter(XDbgHelper::appUnhandledExceptionFilter);
	}

	static void print(const uint8_t* data, int len, const XString& head = XString(), const XString& tail = XString("\n")){
		static std::mutex mtx;
		XMutexLocker mutexLocker(&mtx);
		std::wcout.imbue(std::locale(""));
		std::wcout << head;
		std::wcout << std::hex;
		for (int i = 0; i < len; ++i)
			std::wcout << (int)data[i] << L" ";
		std::wcout << tail;
	}
	static void print(const XString& text){
		//static std::mutex mtx;
		//XMutexLocker locker(&mtx);
		//std::wcout.imbue(std::locale(""));
		//std::wcout << text;

		OutputDebugString(text.c_str());
	}

private:
	XDbgHelper(){}
    static LONG WINAPI appUnhandledExceptionFilter(EXCEPTION_POINTERS *pException)
    {
        TCHAR szModuleFileName[255] = TEXT("");
        TCHAR szDateTime[255] = TEXT("");
        GetModuleFileName(NULL, szModuleFileName, 255);
        SYSTEMTIME stDateTime;
        GetLocalTime(&stDateTime);
        wsprintf(szDateTime, TEXT("%04d%02d%02d%02d%02d%02d"),
            stDateTime.wYear, stDateTime.wMonth, stDateTime.wDay,
            stDateTime.wHour, stDateTime.wMinute, stDateTime.wSecond);
        TCHAR szFileName[255] = TEXT("");
        wsprintf(szFileName, TEXT("%s%s.dmp"), szModuleFileName, szDateTime);
        HANDLE hDumpFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = ::GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        ::MiniDumpWriteDump(
            ::GetCurrentProcess(),
            ::GetCurrentProcessId(),
            hDumpFile,
            MiniDumpNormal,
            &dumpInfo,
            NULL, NULL
            );

        FatalAppExit(0, TEXT("发生不可恢复的错误！\n程序将自动关闭，请联系我们的工程师，我们将尽快处理！"));
        return EXCEPTION_EXECUTE_HANDLER;
    }
	
};

#define XDBG	XDbgHelper
#define xPrint(msg)	XDbgHelper::print(msg)

#endif //_X_DBGHELP_HPP
