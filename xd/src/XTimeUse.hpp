#ifndef _X_TIME_USE_HPP
#define _X_TIME_USE_HPP

#include <string>
#include <ctime>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

typedef void (*X_TIME_PRINT_FUNC)(const std::string& str);

class XTimeUse
{
public:
	XTimeUse(const std::string& id)
		:m_id(id){
		m_tabCount = staticTabCount();
		++staticTabCount();
		m_startClock = m_preClock = clock();
		print("BEGIN",-1);
	}
	~XTimeUse()
	{
		--staticTabCount();
		print("END", elapsed(m_startClock, clock()));
	}
	void print(const std::string& text)
	{
		print(text,restart());
	}
	int restart()
	{
		clock_t  t = clock();
		int ret = elapsed(m_preClock, t);
		m_preClock = t;
		return ret;
	}

private:
	void print(const std::string& text, int t /* =-1 */)
	{
		std::string str =std::string("X_TIME_USE: ")+ std::string(m_tabCount, '\t') + m_id + std::string(": ") + text;
		if (t >= 0) {
			std::stringstream ss;
			ss << t;
			str.append(std::string("\t") + ss.str() + std::string("ms"));
		}
		staticPrintFunc()(str+"\n");
	}
	static int& staticTabCount() { static int tabCount = 0; return tabCount; }
	static X_TIME_PRINT_FUNC& staticPrintFunc() { 
		static X_TIME_PRINT_FUNC printfFunc = XTimeUse::staticPrint;
		return printfFunc; 
	}
	static void staticPrint(const std::string& str)
	{
#ifdef _WIN32
		OutputDebugStringA(str.c_str());
#endif // _WIN32
	}
	static int elapsed(clock_t begin, clock_t end)
	{
		return (end - begin) * 1000 / CLOCKS_PER_SEC;
	}

private:
	std::string m_id;
	int m_tabCount;
	clock_t	m_startClock;
	clock_t m_preClock;
};

#define X_TIME_USE()	XTimeUse _x_time_use(__FUNCTION__)
#define X_TIME_USE_PRINT(str)	_x_time_use.print(str)

#endif //_X_TIME_USE_HPP
