#ifndef _X_NUMBER_SYSTEM_HPP
#define _X_NUMBER_SYSTEM_HPP

#include <cassert>
#include "XMath.hpp"

class XRealNumber
{
public:
	XRealNumber(double d):m_data(d){}
	virtual ~XRealNumber(){}

	operator double() const{ return m_data; }

	friend XRealNumber operator+(const XRealNumber& n1, const XRealNumber& n2)
	{ return n1.m_data + n2.m_data; }
	friend XRealNumber operator-(const XRealNumber& n1, const XRealNumber& n2)
	{ return n1.m_data - n2.m_data; }
	friend XRealNumber operator*(const XRealNumber& n1, const XRealNumber& n2)
	{ return n1.m_data * n2.m_data; }
	friend XRealNumber operator/(const XRealNumber& n1, const XRealNumber& n2)
	{ return n1.m_data / n2.m_data; }
	friend XRealNumber operator+(const XRealNumber& n)
	{ return n.m_data; }
	friend XRealNumber operator-(const XRealNumber& n)
	{ return -n.m_data; }

private:
	double m_data;
};

class XRationalNumber : public XRealNumber
{
public:
	XRationalNumber(int numberator, int denominator = 1) :XRealNumber(numberator*1.0 / denominator)
		, m_numberator(numberator), m_denominator(denominator) {}
	operator double() const { return toDouble(); }

	double toDouble()const { assert(m_denominator != 0); return m_numberator*1.0 / m_denominator; }

	int numberator()const { return m_numberator; }
	int denominator()const { return m_denominator; }

	XRationalNumber simplified()const
	{
		assert(isValid());
		int numberator = m_numberator;
		int denominator = m_denominator;
		if (denominator < 0)
		{
			denominator = -denominator;
			numberator = -numberator;
		}

		int gcd = XMath::GCD(xAbs( numberator), xAbs( m_denominator));
		return XRationalNumber(numberator/gcd, denominator/gcd);
	}

	friend XRationalNumber operator+(const XRationalNumber& n1, const XRationalNumber& n2)
	{
		assert(n1.isValid());
		assert(n2.isValid());

		int lcm = XMath::LCM(xAbs(n1.denominator()), xAbs(n2.denominator()));
		return XRationalNumber(n1.numberator()*(lcm / n1.denominator()) 
			+ n2.numberator()*(lcm / n2.denominator()), lcm).simplified();
	}
	friend XRationalNumber operator-(const XRationalNumber& n1, const XRationalNumber& n2)
	{
		return n1 + (-n2);
	}
	friend XRationalNumber operator*(const XRationalNumber& n1, const XRationalNumber& n2)
	{
		assert(n1.isValid());
		assert(n2.isValid());

		return XRationalNumber(n1.numberator()*n2.numberator(), n1.denominator()*n2.denominator()).simplified();
	}
	friend XRationalNumber operator/(const XRationalNumber& n1, const XRationalNumber& n2)
	{
		assert(n1.isValid());
		assert(n2.isValid());

		return XRationalNumber(n1.numberator()*n2.denominator(),n1.denominator()*n2.numberator()).simplified();
	}
	friend XRationalNumber operator+(const XRationalNumber& n)
	{
		return n;
	}
	friend XRationalNumber operator-(const XRationalNumber& n)
	{
		assert(n.isValid());

		int numberator = -n.numberator();
		int denominator = n.denominator();
		if (denominator < 0) {
			numberator = -numberator;
			denominator = -denominator;
		}
		return XRationalNumber(numberator, denominator);
	}

	bool isValid()const { return m_denominator != 0; }

private:
	int m_numberator;
	int m_denominator;
};

#endif // _X_NUMBER_SYSTEM_HPP

