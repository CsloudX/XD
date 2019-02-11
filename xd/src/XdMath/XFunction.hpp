#ifndef _X_FUNCTION_HPP
#define _X_FUNCTION_HPP

#include <assert.h>
#include "XMath.hpp"
#include "../XString.hpp"

class XFunctionNumber
{

public:
	enum	Type { InfinitesimalType, NumberType, InfiniteType };
	XFunctionNumber(double n=0):m_number(n),m_type(NumberType){}
	Type type()const { return m_type; }
	XString toString()const
	{
		switch (m_type)
		{
		case XFunctionNumber::InfinitesimalType:
			return L"-0";
			break;
		case XFunctionNumber::NumberType:
			return XString::number(m_number);
			break;
		case XFunctionNumber::InfiniteType:
			return L"+0";
			break;
		}
		X_ASSERT(false,"Invalid type");
		return XString();
	}
	static XFunctionNumber Infinitesimal()
	{
		return XFunctionNumber(InfinitesimalType);
	}
	static XFunctionNumber Infinite()
	{
		return XFunctionNumber(InfiniteType);
	}

	friend XFunctionNumber operator+(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		if (n1.m_type==NumberType && n2.m_type==NumberType){
			return XFunctionNumber(n1.m_number + n2.m_number);
		}

		assert(false);
		if (n1.m_type == InfinitesimalType || n2.m_type == InfinitesimalType) {
			return XFunctionNumber::Infinitesimal();
		}
		if (n1.m_type == InfiniteType || n2.m_type == InfiniteType) {
			return XFunctionNumber::Infinite();
		}
		return XFunctionNumber();
	}
	friend XFunctionNumber operator-(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{ return n1 + (-n2); }
	friend XFunctionNumber operator*(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		if (n1.m_type == NumberType && n2.m_type == NumberType) {
			return XFunctionNumber(n1.m_number * n2.m_number);
		}
		assert(false);
		return XFunctionNumber();
	}
	friend XFunctionNumber operator/(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		if (n1.m_type == NumberType && n2.m_type == NumberType) {
			return XFunctionNumber(n1.m_number / n2.m_number);
		}
		assert(false);
		return XFunctionNumber();
	}
	friend XFunctionNumber operator-(const XFunctionNumber& n)
	{
		if (n.m_type == NumberType) {
			return XFunctionNumber(-n.m_number);
		}
		assert(false);
		return XFunctionNumber();
	}
	friend bool operator==(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		if (n1.m_type == NumberType && n2.m_type == NumberType) {
			return (n1.m_number == n2.m_number);
		}

		// 要个极小，要么极大
		return (n1.m_type == n2.m_type);
	}
	friend bool operator!=(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		return !(n1 == n2);
	}
	friend bool operator>(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		if (n1.m_type == NumberType && n2.m_type == NumberType) {
			return (n1.m_number > n2.m_number);
		}
		if (n1.m_type == InfiniteType && n2.m_type != InfiniteType) {
			return true;
		}
		return false;
	}
	friend bool operator<(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		if (n1.m_type == NumberType && n2.m_type == NumberType) {
			return (n1.m_number < n2.m_number);
		}
		if (n1.m_type == InfinitesimalType && n2.m_type != InfinitesimalType) {
			return true;
		}
		return false;
	}
	friend bool operator<=(const XFunctionNumber& n1, const XFunctionNumber& n2)
	{
		if (n1.m_type == NumberType && n2.m_type == NumberType) {
			return (n1.m_number <= n2.m_number);
		}
		if (n1.m_type == InfinitesimalType && n2.m_type != InfinitesimalType) {
			return true;
		}
		return false;
	}
	//friend bool operator>=(const XFunctionNumber& n1, const XFunctionNumber& n2)
	//{
	//	if (n1.m_type == NumberType && n2.m_type == NumberType) {
	//		return (n1.m_number >= n2.m_number);
	//	}
	//	if (n1.m_type == InfiniteType && n2.m_type != InfiniteType) {
	//		return true;
	//	}
	//	return false;
	//}

private:
	XFunctionNumber(Type type, double n=0):m_type(type),m_number(n){}

private:
	double	m_number;
	Type	m_type;
};

class XFunctionRange
{
private:
	XFunctionNumber m_min;
	XFunctionNumber m_max;
	bool m_containMin;
	bool m_containMax;

public:
	XFunctionRange(const XFunctionNumber& minValue = XFunctionNumber::Infinitesimal(),
		const XFunctionNumber& maxValue = XFunctionNumber::Infinite(),
		bool containMin=true,bool containMax=true)
	{
		setRange(minValue, maxValue, containMin, containMax);
	}
	void setRange(const XFunctionNumber& minValue, const XFunctionNumber& maxValue, 
		bool containMin = true, bool containMax = true)
	{
		assert(minValue <= maxValue);
		m_min = minValue;
		m_max = maxValue;
		m_containMin = (m_min.type() == XFunctionNumber::NumberType) ? containMin : false;
		m_containMax = (m_max.type() == XFunctionNumber::NumberType) ? containMin : false;
	}
	void setMinValue(const XFunctionNumber& minValue,bool contain=true) 
	{ setRange(minValue, m_max,contain,m_containMax); }
	void setMaxValue(const XFunctionNumber& maxValue, bool contain = true)
	{ setRange(m_min, maxValue,m_containMin,contain); }

	const XFunctionNumber& minValue()const { return m_min; }
	const XFunctionNumber& maxValue()const { return m_max; }
	bool containMin()const { return m_containMin; }
	bool containMax()const { return m_containMax; }

	XString toString()const
	{
		return XString(L"%1%2,%3%4")
			.arg(m_containMin ? L"[" : L"(")
			.arg(m_min.toString())
			.arg(m_max.toString())
			.arg(m_containMax ? L"]" : L")");
	}
};

class XAbstractFunction
{
public:
	virtual ~XAbstractFunction() {}

protected:
	void setDomain(const XFunctionRange& range) { m_domain = range; }
	void setRange(const XFunctionRange& range) { m_range = range; }

private:
	XFunctionRange	m_domain;
	XFunctionRange	m_range;
};

#endif //_X_FUNCTION_HPP
