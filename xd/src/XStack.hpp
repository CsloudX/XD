#ifndef X_STACK_HPP
#define X_STACK_HPP

#include "XArray.hpp"

template<typename T>
class XStack : public XArray<T>
{
public:
	XStack(int bufferSize=0):XArray(bufferSize){}
	void push(const T& value) { append(value); }
	T pop() { T ret = last(); removeLast(); return ret; }
};

#endif // X_STACK_HPP
