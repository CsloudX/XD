#ifndef _X_SORT_HPP
#define _X_SORT_HPP

#include "XUnitTest.hpp"
#include <vector>
#include <list>

class XSort
{
public:
	template<typename It>
	static void insertSort(It begin, It end) { insertSort(begin, end, std::less<>()); }
	template<typename It, typename Al>
	static void insertSort(It begin, It end, Al al)
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

	template<typename It>
	static void mergeSort(It begin, It end) { mergeSort(begin, end, std::less<>()); }
	template<typename It, typename Al>
	static void mergeSort(It begin, It end, Al al)
	{
		int count = end - begin;
		if (count <= 1) {
			return;
		}

		It mid = begin + count / 2;
		mergeSort(begin, mid, al);
		mergeSort(mid, end, al);
		merge(begin, mid, end, al);
	}

	template<typename It>
	static void heapSort(It begin, It end) { heapSort(begin, end, std::less<>()); }
	template<typename It, typename Al>
	static void heapSort(It begin, It end, Al al){}

	template<typename It>
	static void quickSort(It begin, It end) { quickSort(begin, end, std::less<>()); }
	template<typename It, typename Al>
	static void quickSort(It begin, It end, Al al) {}


private:
	template<typename It, typename Al>
	static void merge(It begin,It mid, It end, Al al)
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



class XSortTest : public XUnitTest
{
protected:
	virtual void testEvent()
	{
		std::vector<int> arr = { 1,3,2,6,8,4,0,9,5,7 };
		std::vector<int> dst = { 0,1,2,3,4,5,6,7,8,9 };

#define _TEST_SORT(al) {std::vector<int> vec=arr;al(vec.begin(),vec.end());X_VERIFY2(vec==dst,#al);}

		_TEST_SORT(XSort::insertSort);
		_TEST_SORT(XSort::mergeSort);
		_TEST_SORT(XSort::heapSort);
		_TEST_SORT(XSort::quickSort);

#undef _TEST_SORT
	}

private:
};

#endif // !_X_SORT_HPP
