#ifndef _X_ALGORITHM_HPP
#define _X_ALGORITHM_HPP

#include "XNamespace.h"
#include <cassert>

template <typename T>
class xLess {
public:
	bool operator()(const T &t1, const T &t2) const { return (t1 < t2); }
};
template <typename T>
class xGreater {
public:
	bool operator()(const T &t1, const T &t2) const { return (t1 > t2); }
};

template <typename ForwardIterator, typename CompareFunc>
inline void xInsertSort(ForwardIterator begin, ForwardIterator end, CompareFunc func)
{
	assert(begin < end;)
	for (ForwardIterator itr = begin + 1; itr < end; ++itr)
	{
		auto value = *itr;
		auto i = itr;
		while (i > begin && func(value, *(i - 1))) {
			*i = *(i - 1);
			--i;
		}
		*i = value;
	}
}

template<typename ForwardIterator, typename T>
inline std::vector<T> xGenVector (ForwardIterator begin, ForwardIterator end, const T& dummy)
{
	int count = end - begin;
	std::vector<T> ret(count);
	for (int i = 0; i < count; ++i) {
		ret[i] = *(begin + i);
	}
	return ret;
}

template <typename ForwardIterator, typename CompareFunc>
inline void xMerge(ForwardIterator begin, ForwardIterator mid, ForwardIterator end, CompareFunc func)
{
	assert(begin <= mid);
	assert(mid <= end);

	auto l1 = xGenVector(begin, mid, *begin);
	auto l2 = xGenVector(mid, end, *begin);


	ForwardIterator itr = begin;
	auto itr1 = l1.begin();
	auto itr2 = l2.begin();
	while (itr1 != l1.end() && itr2 != l2.end())
	{
		if (func(*itr1, *itr2)) {
			*itr = *itr1;
			++itr1;
		}
		else {
			*itr = *itr2;
			++itr2;
		}
		++itr;
	}

	while (itr1 != l1.end()) {
		*itr = *itr1;
		++itr1;
		++itr;
	}
	while (itr2 != l2.end()) {
		*itr = *itr2;
		++itr2;
		++itr;
	}
}

template <typename ForwardIterator, typename CompareFunc>
inline void xMergeSort(ForwardIterator begin, ForwardIterator end, CompareFunc func)
{
	if (begin < end-1) {
		auto mid = begin + (end - begin) / 2;
		xMergeSort(begin, mid, func);
		xMergeSort(mid, end, func);
		xMerge(begin, mid, end, func);
	}
}

template <typename Iterator, typename T>
inline T xFindMaxCrossSubArray(Iterator* begin,Iterator* mid, Iterator* end, const T& dummy)
{
	auto _begin = *begin;
	auto _mid = *mid;
	auto _end = *end;
	assert(_begin < _mid && _mid < _end);

	Iterator leftPos=_mid;
	T leftMaxValue = 0;
	T leftVal = 0;
	for (auto pos = _mid-1; pos>=_begin;--pos) {
		leftVal += *pos;
		if (leftVal > leftMaxValue) {
			leftMaxValue = leftVal;
			leftPos = pos;
		}
	}

	Iterator rightPos = _mid;
	T rightMaxValue = 0;
	T rightValue=0;
	for (auto pos = _mid; pos < _end; ++pos) {
		rightValue += *pos;
		if (rightValue > rightMaxValue) {
			rightMaxValue = rightValue;
			rightPos = pos;
		}
	}

	*begin = leftPos;
	*end = rightPos + 1;
	return leftMaxValue + rightMaxValue;

}

template <typename Iterator, typename T>
inline T xFindMaxSubArray(Iterator* begin, Iterator* end, const T& dummy)
{
	auto _begin = *begin;
	auto _end = *end;
	assert(_begin < _end);

	if (_begin == _end - 1)
		return *_begin;

	auto _mid = _begin +(_end - _begin) / 2;
	auto leftBegin = _begin;
	auto leftEnd = _mid;
	auto midBegin = _begin;
	auto midMid = _mid;
	auto midEnd = _end;
	auto rightBegin = _mid;
	auto rightEnd = _end;

	auto leftVal = xFindMaxSubArray(&leftBegin, &leftEnd, *leftBegin);
	auto rightVal = xFindMaxSubArray(&rightBegin, &rightEnd, *rightBegin);
	auto midVal = xFindMaxCrossSubArray(&midBegin, &midMid, &midEnd, *midBegin);
	if (leftVal >= rightVal && leftVal >= midVal) {
		*begin = leftBegin;
		*end = leftEnd;
		return leftVal;
	}else if (rightVal >= leftVal && rightVal >= midVal) {
		*begin = rightBegin;
		*end = rightEnd;
		return rightVal;
	}
	else if (midVal >= leftVal && midVal >= rightVal) {
		*begin = midBegin;
		*end = midEnd;
		return midVal;
	}

	assert(false);
	return *_begin;
}


#endif //_X_ALGORITHM_HPP
