#ifndef _X_TIME_RECORDER_HPP
#define _X_TIME_RECORDER_HPP

#include <XTime.hpp>
#include <win32\XDbgHelper.hpp>

class XTimeRecorder
{
public:
	XTimeRecorder(const XString& id = XString()) :m_id(id), m_timeUsed(0)
	{
		m_time = XTime::currentTime();
		xPrint(XString(L"XTimeRecorder(%1) start.\n").arg(m_id));
	}
	~XTimeRecorder()
	{
		print(L"end");
	}

	void print(const XString& msg)
	{
		int t = m_time.restart();
		m_timeUsed += t;
		xPrint(XString(L"%1(%2): Time Used: %3 ms. Time Elapsed: %4 ms.\n").arg(m_id).arg(msg).arg(t).arg(m_timeUsed));
	}

private:
	XTime	m_time;
	XString	m_id;
	int		m_timeUsed;
};

#endif //_X_TIME_RECORDER_HPP
