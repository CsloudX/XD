#ifndef _X_RANDOM_HPP
#define _X_RANDOM_HPP

#include <cstdlib>
#include <ctime>
#include <cassert>

class XRandom 
{
public:
	XRandom() 
	{ 
		static bool inited = false;
		if (!inited) {
			srand(time(0));
			inited = true;
		} 
	}
	XRandom(int seed) { srand(seed); }
	int next() { return rand(); }
	int next(int maxValue) { return rand() % maxValue; }
	int next(int minValue, int maxValue)
	{
		assert(minValue < maxValue);
		return next(maxValue - minValue) + minValue;
	}
	double next(double maxValue)
	{
		return rand()*maxValue / RAND_MAX;
	}
	double next(double minValue, double maxValue)
	{
		assert(minValue < maxValue);
		return next(maxValue - minValue) + minValue;
	}
};

#endif // !_X_RANDOM_HPP
