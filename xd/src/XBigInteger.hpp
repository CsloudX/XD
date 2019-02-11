#ifndef _X_BIG_INTEGER_HPP
#define _X_BIG_INTEGER_HPP

#include <vector>
#include "XMath.hpp"
#include "XUnitTest.hpp"

// XBigIntegerS必定是个正整数
class XBigIntegerS
{
public:
	XBigIntegerS() { init(0, 10); }
	XBigIntegerS(int n, int base) { init(n, base); }
	XBigIntegerS(const std::string& str, int base) { init(str, base); }
	void init(int n, int base)
	{
		assert(n>=0 && base>0);

		m_data.clear();
		m_base = base;
		while (n >= m_base)
		{
			m_data.push_back(n%m_base);
			n = n / m_base;
		}
		m_data.push_back(n);
	}
	void init(const std::string& str, int base)
	{
		assert(base>0);

		m_data.clear();
		m_base = base;
		int count = str.size();
		for (int i = count - 1; i >= 0; --i)
		{
			int bitVal = toInt(str.at(i));
			assert(0 <= bitVal && bitVal < m_base);
			m_data.push_back(bitVal);
		}
	}

	int base()const { return m_base; }

	int toInt()const
	{
		int ret = 0;
		int k = 1;
		for (int n : m_data)
		{
			ret += (k*n);
			k = k*m_base;
		}
		return ret;
	}
	std::string toString()const
	{
		std::string ret;
		int count = (XMath::max)(size(),1);
		ret.resize(count);
		for (int i = 0; i < count; ++i)
		{
			ret[i] = toChar(m_data.at(count - i - 1));
		}
		return ret;
	}

	int bitValue(int i)const 
	{ 
		assert(i >= 0); 

		if (i < (int)m_data.size())
			return m_data.at(i);

		return 0;
	}
	void setBitValue(int i, int value)
	{
		assert(i >= 0 && value >= 0);

		int oldSZ = m_data.size();
		if (i >= oldSZ)
		{
			m_data.resize(i + 1);
			for (int j = oldSZ; j < i; ++j) {
				m_data[j] = 0;
			}
		}
		m_data[i] = value;
	}
	void addBitValue(int i, int value)
	{
		assert(i >= 0 && value >= 0);

		int bitVal = bitValue(i)+value;
		if (bitVal >= m_base) {
			setBitValue(i + 1, bitVal / m_base);
			bitVal = bitVal%m_base;
		}

		assert(0 <= bitVal && bitVal < m_base);
		setBitValue(i, bitVal);
	}
	void subBitValue(int i, int value)
	{
		assert(0<=i && i<(int)m_data.size() && 0<=value&&value<m_base);

		int bitVal = bitValue(i) - value;
		if (bitVal < 0) {
			bitVal += m_base;
			subBitValue(i + 1, 1);
		}
		setBitValue(i, bitVal);
	}

	XBigIntegerS divide(const XBigIntegerS& divisor, XBigIntegerS* remainder)const
	{
		assert(m_base == divisor.m_base && divisor!=XBigIntegerS(0,m_base));

		XBigIntegerS base(m_base, m_base);
		XBigIntegerS ret(0, m_base);
		XBigIntegerS m(0,m_base);

		for (int i = size(); i >= 0; --i) 
		{
			ret = ret * base;
			m = m*base + XBigIntegerS(bitValue(i),m_base);
			if (m >= divisor) {
				ret = ret + m.divideS(divisor, remainder);
				m = *remainder;
			}
		}

		if (remainder) {
			*remainder = m;
		}
		return ret;
	}

	void changeBase(int base)
	{
		assert(0 < base);
		if (m_base == base)
			return;

		// m / n = k .. r
		XBigIntegerS m = *this;
		XBigIntegerS n(base, m_base);
		XBigIntegerS r;

		m_data.clear();
		while (m >= n)
		{
			m = m.divide(n, &r);
			m_data.push_back(r.toInt());
		}
		m_data.push_back(m.toInt());
		m_base = base;
	}

	friend bool operator == (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);
		int count = (XMath::max)(a.size(), b.size());
		for (int i = 0; i < count; ++i)
		{
			if (a.bitValue(i) != b.bitValue(i))
				return false;
		}
		return true;
	}
	friend bool operator != (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);
		int count = (XMath::max)(a.size(), b.size());
		for (int i = 0; i < count; ++i)
		{
			if (a.bitValue(i) != b.bitValue(i))
				return true;
		}
		return false;
	}
	friend bool operator < (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);
		int count = (XMath::max)(a.size(), b.size());
		for (int i = count; i >=0 ; --i)
		{
			if (a.bitValue(i) < b.bitValue(i))
				return true;
			if (a.bitValue(i) > b.bitValue(i))
				return false;
		}
		return false;
	}

	friend bool operator <= (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);
		int count = (XMath::max)(a.size(), b.size());
		for (int i = count; i >= 0; --i)
		{
			if (a.bitValue(i) < b.bitValue(i))
				return true;
			if (a.bitValue(i) > b.bitValue(i))
				return false;
		}
		return true;
	}
	friend bool operator > (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);
		int count = (XMath::max)(a.size(), b.size());
		for (int i = count; i >= 0; --i)
		{
			if (a.bitValue(i) > b.bitValue(i))
				return true;

			if (a.bitValue(i) < b.bitValue(i))
				return false;
		}
		return false;
	}
	friend bool operator >= (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);
		int count = (int)(XMath::max)(a.size(), b.size());
		for (int i = count; i >= 0; --i)
		{
			if (a.bitValue(i) > b.bitValue(i))
				return true;

			if (a.bitValue(i) < b.bitValue(i))
				return false;
		}
		return true;
	}

	friend XBigIntegerS operator + (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);

		XBigIntegerS ret(0, a.m_base);
		int count = (XMath::max)(a.size(), b.size());
		for (int i = 0; i < count; i++)
		{
			ret.addBitValue(i, a.bitValue(i) + b.bitValue(i));
		}
		return ret;
	}

	friend XBigIntegerS operator - (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base && a >= b);

		XBigIntegerS ret = a;
		int size = b.size();
		for (int i = 0; i < size; ++i) {
			ret.subBitValue(i, b.m_data.at(i));
		}
		return ret;
	}
	friend XBigIntegerS operator * (const XBigIntegerS& a, const XBigIntegerS& b)
	{
		assert(a.m_base == b.m_base);

		XBigIntegerS ret(0, a.m_base);
		int aSize = a.size();
		int bSize = b.size();
		for (int i = 0; i < bSize; ++i) {
			int k = b.bitValue(i);
			XBigIntegerS m(0, a.m_base);
			for (int j = 0; j < aSize; ++j){
				m.addBitValue(i + j, k*a.bitValue(j));
			}
			ret = ret+ m;
		}
		return ret;
	}

private:
	XBigIntegerS divideS(const XBigIntegerS& divisor, XBigIntegerS* remainder)const
	{
		assert(m_base == divisor.m_base && divisor != XBigIntegerS(0, m_base));

		XBigIntegerS ret(0, m_base);
		XBigIntegerS m = *this;
		while (m >= divisor)
		{
			m = m - divisor;
			ret = ret + XBigIntegerS(1, m_base);
		}

		if (remainder) {
			*remainder = m;
		}
		return ret;
	}
	int size()const
	{
		for (int i = m_data.size()-1; i >= 0; --i) {
			if (m_data.at(i) > 0)
				return i+1;
		}
		return 0;
	}

	static char toChar(int n)
	{
		assert(0 <= n && n < 16);
		static char chs[] = {
			'0','1','2','3','4','5','6','7',
			'8','9','A','B','C','D','E','F'
		};
		return chs[n];
	}
	static int toInt(char ch)
	{
		if ('0' <= ch && ch <= '9')
			return ch - '0';
		else if ('A' <= ch && ch <= 'F')
			return ch - 'A' + 10;
		
		assert(false);
		return -1;
	}

private:
	int m_base;
	std::vector<int> m_data;
};

class XBigInteger
{
public:
	XBigInteger(int n=0, int base=10) 
	{ 
		m_isNegative = (n < 0);
		m_num.init(m_isNegative ? -n : n, base);
	}
	XBigInteger(const std::string& str, int base)
	{
		m_isNegative = (!str.empty() && str.at(0) == '-');
		m_num.init(m_isNegative?str.substr(1):str, base);
	}

	int base()const { return m_num.base(); }
	int toInt()const { return m_isNegative ? -m_num.toInt() : m_num.toInt(); }
	operator int() const { return toInt(); }
	std::string toString()const { return m_isNegative ? "-" + m_num.toString() : m_num.toString(); }

	XBigInteger divide(const XBigInteger& other, XBigInteger* remainder=nullptr)const
	{
		XBigInteger ret;
		ret.m_isNegative = (m_isNegative != other.m_isNegative);
		XBigIntegerS r;
		ret.m_num = m_num.divide(other.m_num, &r);
		if (remainder) {
			remainder->m_isNegative = false;
			remainder->m_num = r;
		}
		return ret;
	}

	void changeBase(int base) { m_num.changeBase(base); }

	friend XBigInteger operator + (const XBigInteger& a, const XBigInteger& b)
	{
		XBigInteger ret;
		if (a.m_isNegative == b.m_isNegative) {
			ret.m_isNegative = a.m_isNegative;
			ret.m_num = a.m_num + b.m_num;
		}
		else {
			if (a.m_num > b.m_num) {
				ret.m_isNegative = a.m_isNegative;
				ret.m_num = a.m_num - b.m_num;
			}
			else {
				ret.m_isNegative = b.m_isNegative;
				ret.m_num = b.m_num - a.m_num;
			}
		}
		return ret;
	}

	friend XBigInteger operator - (const XBigInteger& a, const XBigInteger& b)
	{
		return a + (-b); 
	}
	friend XBigInteger operator * (const XBigInteger& a, const XBigInteger& b)
	{
		XBigInteger ret;
		ret.m_isNegative = (a.m_isNegative != b.m_isNegative);
		ret.m_num = a.m_num*b.m_num;
		return ret;
	}
	friend XBigInteger operator / (const XBigInteger& a, const XBigInteger& b)
	{
		return a.divide(b);
	}
	friend XBigInteger operator - (const XBigInteger& o)
	{
		XBigInteger ret = o;
		ret.m_isNegative = !ret.m_isNegative;
		return ret;
	}

private:
	bool m_isNegative;
	XBigIntegerS m_num;
};


class XBigIntegerTest: public XUnitTest
{
protected:
	virtual void testEvent()
	{
		X_VERIFY(XBigInteger(123) == 123);
		X_VERIFY(XBigInteger(123).toString() == "123");

		X_OUTPUT(XBigInteger n2(-123));
		X_VERIFY(n2 == -123);
		X_VERIFY(n2.toString() == "-123");

		X_OUTPUT(XBigInteger n3("FE",16));
		X_VERIFY(n3 == 254);
		X_VERIFY(n3.toString() == "FE");

		X_VERIFY(XBigInteger(123) + XBigInteger(-111) == 12);
		X_VERIFY(XBigInteger(123) - XBigInteger(111) == 12);
		X_VERIFY(XBigInteger(56) * XBigInteger(23) == 1288);
		X_VERIFY(XBigInteger(55) / XBigInteger(11) == 5);
		
		XBigInteger FF("FF", 16);
		FF.changeBase(10);
		X_VERIFY(FF.toString() =="255");
	}
};

#endif // _X_BIG_INTEGER_HPP
