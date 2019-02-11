#ifndef X_QUEUE_HPP
#define X_QUEUE_HPP

#include "XDoubleLinkedList.hpp"

template<typename T>
class XQueue : public XDoubleLinkedList<T>
{
public:
	void enqueue(const T& value) { append(value); }
	T dequeue() { T ret = first(); removeFirst(); return ret; }
};

#endif // X_QUEUE_HPP
