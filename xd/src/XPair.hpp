#ifndef X_PAIR_HPP
#define X_PAIR_HPP

template<typename T1,typename T2>
class XPair
{
public:
	T1 first;
	T2 second;

	XPair(const T1& n1, const T2& n2):first(n1),second(n2){}
	XPair(){}
};

template <typename T1, typename T2>
XPair<T1, T2> xMakePair(const T1 &n1, const T2 &n2)
{
	return XPair<T1, T2>(n1, n2);
}

#endif // X_PAIR_HPP
