#ifndef _X_BIG_NUMBER_HPP
#define _X_BIG_NUMBER_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include "XMath.hpp"
#include "XUnitTest.hpp"

// XBigNumberS表示一个无符号的数
class XBigNumberS
{
public:
	XBigNumberS() { init(0, 10); }
	XBigNumberS(double n, int base, int precision=1000) { init(n, 10, precision); }
	XBigNumberS(const std::string& str, int base) { init(str, base); }

	std::string toString()const
	{
		std::string ret;

		if (!m_integer.empty()) {
			std::vector<int>::const_reverse_iterator itr = m_integer.crbegin();
			std::vector<int>::const_reverse_iterator itrEnd = m_integer.crend();
			while (itr != itrEnd) {
				ret.push_back(toChar(*itr++));
			}
		}
		else {
			ret.push_back('0');
		}

		if (!m_decimal.empty()) {
			ret.push_back('.');
			for (int n : m_decimal) {
				ret.push_back(toChar(n));
			}
		}
		return ret;
	}

	friend bool operator == (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base);

		if (a.m_integer.size() != b.m_integer.size())	return false;
		if (a.m_decimal.size() != b.m_decimal.size())	return false;

		int i = -(int)(a.m_decimal.size());
		int iEnd = a.m_integer.size();
		for (; i < iEnd; ++i)
		{
			if (a.bitValue(i) != b.bitValue(i))
				return false;
		}
		return true;
	}
	friend bool operator != (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base);

		if (a.m_integer.size() != b.m_integer.size())	return true;
		if (a.m_decimal.size() != b.m_decimal.size())	return true;

		int i = -(int)(a.m_decimal.size());
		int iEnd = a.m_integer.size();
		for (; i < iEnd; ++i)
		{
			if (a.bitValue(i) != b.bitValue(i))
				return true;
		}
		return false;
	}
	friend bool operator < (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base);

		int iMin = -(int)((XMath::max)(a.m_decimal.size(), b.m_decimal.size()));
		int iMax = (XMath::max)(a.m_integer.size(), b.m_integer.size());
		for (int i=iMax;i>=iMin;--i)
		{
			if (a.bitValue(i) < b.bitValue(i))
				return true;
			if (a.bitValue(i) > b.bitValue(i))
				return false;
		}
		return false;
	}
	friend bool operator <= (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base);

		int iMin = -(int)((XMath::max)(a.m_decimal.size(), b.m_decimal.size()));
		int iMax = (XMath::max)(a.m_integer.size(), b.m_integer.size());
		for (int i = iMax; i >= iMin; --i)
		{
			if (a.bitValue(i) < b.bitValue(i))
				return true;
			if (a.bitValue(i) > b.bitValue(i))
				return false;
		}
		return true;
	}
	friend bool operator > (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base);

		int iMin = -(int)((XMath::max)(a.m_decimal.size(), b.m_decimal.size()));
		int iMax = (XMath::max)(a.m_integer.size(), b.m_integer.size());
		for (int i = iMax; i >= iMin; --i)
		{
			if (a.bitValue(i) > b.bitValue(i))
				return true;
			if (a.bitValue(i) < b.bitValue(i))
				return false;
		}
		return false;
	}
	friend bool operator >= (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base);

		int iMin = -(int)((XMath::max)(a.m_decimal.size(), b.m_decimal.size()));
		int iMax = (XMath::max)(a.m_integer.size(), b.m_integer.size());
		for (int i = iMax; i >= iMin; --i)
		{
			if (a.bitValue(i) > b.bitValue(i))
				return true;
			if (a.bitValue(i) < b.bitValue(i))
				return false;
		}
		return true;
	}

	friend XBigNumberS operator + (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base);

		XBigNumberS ret(0, a.m_base);
		int i = -(int)((XMath::max)(a.m_decimal.size(), b.m_decimal.size()));
		int iEnd = (XMath::max)(a.m_integer.size(), b.m_integer.size());
		for (; i < iEnd; ++i)
		{
			ret.addBitValue(i, a.bitValue(i) + b.bitValue(i));
		}
		ret.normalize();

		return ret;
	}

	friend XBigNumberS operator - (const XBigNumberS& a, const XBigNumberS& b)
	{
		assert(a.m_base == b.m_base && a>=b);

		XBigNumberS ret = a;
		int i = -(int)((XMath::max)(a.m_decimal.size(), b.m_decimal.size()));
		int iEnd = (XMath::max)(a.m_integer.size(), b.m_integer.size());
		for (; i < iEnd; ++i)
		{
			ret.subBitValue(i, b.bitValue(i));
		}
		ret.normalize();

		return ret;
	}

private:
	void init(double n, int base, int precision)
	{
		assert(base > 1 && n >= 0 && precision>=0);
		m_base = base;

		int integer = XMath::floor(n);
		double decimal = n - integer;
		
		while (integer >= m_base) {
			m_integer.push_back(integer%m_base);
			integer = integer / m_base;
		}
		m_integer.push_back(integer);

		while (decimal != 0 && (int)m_decimal.size()<precision) {
			decimal = decimal*m_base;
			integer = XMath::floor(decimal);
			decimal = decimal - integer;
			m_decimal.push_back(integer);
		}

		normalize();
	}
	void init(const std::string& str, int base)
	{
		assert(base > 1);
		m_base = base;

		size_t totalSize = str.size();
		size_t decimalPos = str.find('.');
		if (decimalPos == std::string::npos) {
			decimalPos = totalSize;
		}

		m_integer.resize(decimalPos);
		for (size_t i = 0; i < decimalPos; ++i) {
			m_integer[i] = toInt( str.at(decimalPos - i - 1));
		}

		m_decimal.resize((XMath::max)((size_t)0, totalSize - decimalPos - 1));
		for (size_t i = decimalPos + 1; i < totalSize; ++i) {
			m_decimal[i - decimalPos - 1] = toInt(str.at(i));
		}

		normalize();
	}

	void setBitValue(int i, int value)
	{
		assert(value >= 0);

		if (i >= 0) {
			setBitValue(&m_integer, i, value);
		}
		else {
			setBitValue(&m_decimal, -i-1, value);
		}
	}
	int bitValue(int i) const
	{
		if (i >= 0) {
			return bitValue(m_integer, i);
		}
		else {
			return bitValue(m_decimal, -i-1);
		}
	}
	void addBitValue(int i, int value)
	{
		assert(value >= 0);

		int rsValue = bitValue(i) + value;
		if (rsValue >= m_base) {
			setBitValue(i + 1, rsValue / m_base);
			rsValue = rsValue%m_base;
		}

		assert(0 <= rsValue&&rsValue < m_base);
		setBitValue(i, rsValue);
	}

	void subBitValue(int i, int value)
	{
		assert(value >= 0);

		int rsValue = bitValue(i) - value;
		if (rsValue < 0) {
			rsValue += m_base;
			subBitValue(i + 1, 1);
		}

		setBitValue(i, rsValue);
	}

	void normalize()
	{
		normalize(&m_integer);
		normalize(&m_decimal);
	}

	static int toInt(char ch)
	{
		if ('0' <= ch && ch <= '9') {
			return ch - '0';
		}
		if ('A' <= ch && ch <= 'Z') {
			return ch - 'A' + 10;
		}
		assert(false);
		return -1;
	}
	static char toChar(int n)
	{
		assert(0<=n&&n<36);
		static char chs[] = {
			'0','1','2','3','4','5','6','7', '8','9',
			'A','B','C','D','E','F','G',
			'H','I','J','K','L','M','N',
			'O','P','Q','R','S','T',
			'U','V','W','X','Y','Z'
		};
		return chs[n];
	}
	static void setBitValue(std::vector<int>* vec, int i, int value)
	{
		assert(i >= 0 && value >= 0);

		if ((size_t)i >= vec->size()) {
			int preSize = vec->size();
			vec->resize(i + 1);
			for (int j = preSize; j < i; ++j) {
				(*vec)[j] = 0;
			}
		}
		(*vec)[i] = value;
	}
	static int bitValue(const std::vector<int>& vec, int i)
	{
		assert(i >= 0);
		if ((size_t)i < vec.size()) {
			return vec.at(i);
		}
		return 0;
	}
	static void normalize(std::vector<int>* vec)
	{
		for (int i = vec->size() - 1; i >= 0; i--) {
			if (vec->at(i) != 0) {
				vec->resize(i + 1);
				return;
			}
		}
		vec->clear(); 
	}

private:
	int m_base;
	std::vector<int> m_integer;
	std::vector<int> m_decimal;
};


#endif // _X_BIG_NUMBER_HPP
