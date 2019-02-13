#ifndef _X_SYSTEM_INFO_HPP
#define _X_SYSTEM_INFO_HPP

#include <Windows.h>
#include <QDebug>
class XSystemInfo
{
public:
	XSystemInfo()
	{
		GetSystemTimes(&m_preIdleTime, &m_preKernelTime, &m_preUserTime);
	}
	
	double calcCpuUsage()
	{
		FILETIME idleTime, kernelTime, userTime;
		if (GetSystemTimes(&idleTime, &kernelTime, &userTime) == FALSE)
			return -1;

		__int64 valIdle = compareFileTime(m_preIdleTime, idleTime);
		__int64 valKernel = compareFileTime(m_preKernelTime, kernelTime);
		__int64 valUser = compareFileTime(m_preUserTime, userTime);
		
		double ret = 0;
		if ((valKernel + valUser) > 0)
		{
			ret = (valKernel + valUser - valIdle) * 100.0 / (valKernel + valUser) + 0.5;
		}

		m_preIdleTime = idleTime;
		m_preKernelTime = kernelTime;
		m_preUserTime = userTime;
		return ret;
	}

private:
	static __int64 compareFileTime(const FILETIME& t1, const FILETIME& t2)
	{
		return ((__int64)t2.dwHighDateTime << 32 |t2.dwLowDateTime)
			- ((__int64)t1.dwHighDateTime << 32 | t1.dwLowDateTime); 
	}

private:
	FILETIME m_preIdleTime;
	FILETIME m_preKernelTime;
	FILETIME m_preUserTime;
};

#endif //_X_SYSTEM_INFO_HPP
