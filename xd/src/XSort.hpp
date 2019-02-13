#ifndef _X_SORT_HPP
#define _X_SORT_HPP

#include "XUnitTest.hpp"
#include <vector>
#include <list>

class XSortAlgorithm
{
protected:
	template<typename It>
	static void swap(It a, It b)
	{
		std::_Iter_value_t<It> t = *a;
		*a = *b;
		*b = t;
	}
};

class XInsertSort : public XSortAlgorithm
{
public:
	template<typename It, typename Al>
	XInsertSort(It begin, It end, Al al) { (*this)(begin, end, al); }

	template<typename It>
	XInsertSort(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It>
	void operator()(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It, typename Al>
	void operator()(It begin, It end, Al al)
	{
		for (It itr = begin; itr != end; ++itr)
		{
			std::_Iter_value_t<It> m = *itr;
			It itr2 = itr;
			while (itr2 > begin)
			{
				std::_Iter_value_t<It> n = *(itr2 - 1);
				if (al(m, n)) {
					*itr2 = n;
				}
				else {
					break;
				}
				--itr2;
			}
			*itr2 = m;
		}
	}
};

class XMergeSort : public XSortAlgorithm
{
public:
	template<typename It, typename Al>
	XMergeSort(It begin, It end, Al al) { (*this)(begin, end, al); }

	template<typename It>
	XMergeSort(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It>
	void operator()(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It, typename Al>
	void operator()(It begin, It end, Al al)
	{
		int count = end - begin;
		if (count <= 1) {
			return;
		}

		It mid = begin + count / 2;
		(*this)(begin, mid, al);
		(*this)(mid, end, al);
		merge(begin, mid, end, al);
	}

private:
	template<typename It, typename Al>
	static void merge(It begin, It mid, It end, Al al)
	{
		std::list<std::_Iter_value_t<It> > arr1 = toList(begin, mid);
		std::list<std::_Iter_value_t<It> > arr2 = toList(mid, end);
		It itr = begin;
		while (!arr1.empty() && !arr2.empty())
		{
			if (al(arr1.front(), arr2.front())) {
				*itr = arr1.front();
				arr1.pop_front();
			}
			else {
				*itr = arr2.front();
				arr2.pop_front();
			}
			++itr;
		}
		while (!arr1.empty()) {
			*itr = arr1.front();
			arr1.pop_front();
			++itr;
		}
		while (!arr2.empty()) {
			*itr = arr2.front();
			arr2.pop_front();
			++itr;
		}
	}
	template<typename It>
	static std::list<std::_Iter_value_t<It> > toList(It begin, It end)
	{
		std::list<std::_Iter_value_t<It> > ret;
		for (It itr = begin; itr != end; ++itr) {
			ret.push_back(*itr);
		}
		return ret;
	}
};

class XHeapSort : public XSortAlgorithm
{
public:
	template<typename It, typename Al>
	XHeapSort(It begin, It end, Al al) { (*this)(begin, end, al); }

	template<typename It>
	XHeapSort(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It>
	void operator()(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It, typename Al>
	void operator()(It begin, It end, Al al)
	{
		
	}
};

class XQuickSort: public XSortAlgorithm
{
public:
	template<typename It,typename Al>
	XQuickSort(It begin, It end, Al al) { (*this)(begin, end, al); }

	template<typename It>
	XQuickSort(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It>
	void operator()(It begin, It end) { (*this)(begin, end, std::less<>()); }

	template<typename It, typename Al>
	void operator()(It begin, It end, Al al)
	{
		if (begin < end) {
			It mid = partition(begin, end, al);
			(*this)(begin, mid, al);
			(*this)(mid + 1, end, al);
		}
	}
private:
	template<typename It, typename Al>
	static It partition(It begin, It end, Al al)
	{
		It jEnd = end - 1;
		std::_Iter_value_t<It> x = *jEnd;
		It i = begin;
		for (; i < jEnd; ++i) {
			if (!al(*i, x)) {
				break;
			}
		}
		It j = i + 1;
		for (; j < jEnd; ++j) {
			if (al(*j, x)) {
				swap(i, j);
				++i;
			}
		}
		swap(i, jEnd);
		return i;
	}
};


class XSortTest : public XUnitTest
{
protected:
	virtual void testEvent()
	{
		std::vector<int> arr = { 1,3,2,6,8,4,0,9,5,7 };
		std::vector<int> dst = { 0,1,2,3,4,5,6,7,8,9 };

#define _TEST_SORT(al) {std::vector<int> vec=arr;al(vec.begin(),vec.end());X_VERIFY2(vec==dst,#al);}

		_TEST_SORT(XInsertSort);
		_TEST_SORT(XMergeSort);
		_TEST_SORT(XHeapSort);
		_TEST_SORT(XQuickSort);

#undef _TEST_SORT
	}

private:
};

#endif // !_X_SORT_HPP
