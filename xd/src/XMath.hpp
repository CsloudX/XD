#ifndef X_MATH_HPP
#define X_MATH_HPP

#include <cmath>
#include <cassert>
#include <map>
#include "XUnitTest.hpp"

class XMath
{
public:
	static double pi()
	{
		static double value = 3.14159265358979323846;
		return value;
	}

	static double rad2deg(double rad)
	{
		static double r = 180 / pi();
		return rad*r;
	}
	static double deg2rad(double deg)
	{
		static double r = pi() / 180;
		return deg*r;
	}

	static double sin(double rad)
	{
		return ::sin(rad);
	}
	static double cos(double rad)
	{
		return ::cos(rad);
	}
	static double sec(double rad)
	{
		return 1.0 / cos(rad);
	}
	static double csc(double rad)
	{
		return 1.0 / sin(rad);
	}
	static double tan(double rad)
	{
		return sin(rad) / cos(rad);
	}
	static double cot(double rad)
	{
		return cos(rad)/sin(rad);
	}
	static double sqrt(double n)
	{
		return ::sqrt(n);
	}
	static double pow(double n, double p)
	{
		return ::pow(n, p);
	}

	static int factorial(int n) 
	{ 
		assert(n >= 0);

		static std::map<int, int> items;

		auto itr = items.find(n);
		if (itr != items.end())
			return itr->second;

		int rs = 0;
		if (n > 1)
			rs = n*factorial(n - 1);
		else if (n == 1 || n == 0)
			rs = 1;
		items[n] = rs;

		return rs;
	}
	static int permutation(int n, int k) 
	{ 
		assert(n >= 0 && k >= 0 && n >= k);
		return factorial(n) / factorial(n - k);
	}
	static int combination(int n, int k)
	{
		assert(n >= 0 && k >= 0 && n >= k);
		return factorial(n) /(factorial(k)* factorial(n - k));
	}

	template <typename T>
	static T abs(const T& t) { return t >= 0 ? t : -t; }

	template <typename T>
	static const T& (max)(const T& a, const T& b) { return (a > b) ? a : b; }

	template <typename T>
	static const T& (min)(const T& a, const T& b) { return (a < b) ? a : b; }

	static int ceil(double n) { return int(n+1); }
	static int floor(double n) { return int(n); }

	template<typename F>
	static double sum(int begin, int end, const F& func)
	{
		assert(begin <= end);

		double ret = 0;
		for (int i = begin; i <= end; ++i)
		{
			ret += func(i);
		}
		return ret;
	}
	template<typename F>
	static double product(int begin, int end, const F& func)
	{
		assert(begin <= end);

		double ret = 1;
		for (int i = begin; i <= end; ++i)
		{
			ret *= func(i);
		}
		return ret;
	}


	static double centigrade2fahrenheit(double c)
	{
		return c*1.8 + 32;
	}
	static double fahrenheit2centigrade(double f)
	{
		static double r = 1.0 / 1.8;
		return (f - 32)*r;
	}

};

class XMathTest :public XUnitTest
{
protected:
	virtual void testEvent()
	{
		X_COMPARE(XMath::factorial(3), 6);
		X_COMPARE(XMath::permutation(3,2), 6);
		X_COMPARE(XMath::combination(3, 2), 3);
		X_COMPARE(XMath::combination(3, 0), 1);
	}
};

#endif //X_MATH_HPP
