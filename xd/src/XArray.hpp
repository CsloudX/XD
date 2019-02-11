#ifndef X_ARRAY_HPP
#define X_ARRAY_HPP

#include <cassert>

template<typename T>
class XArray
{
public:
	class iterator {
	public:
		iterator(T* buffer,int index) :m_buffer(buffer), m_index(index) {}
		T operator*()const { return m_buffer[m_index]; }
		iterator& operator++() { ++m_index; return *this; }
		bool operator==(const iterator& other) {
			assert(m_buffer == other.m_buffer);
			return this->m_index == other.m_index;
		}
		bool operator!=(const iterator& other) { 
			assert(m_buffer == other.m_buffer);
			return this->m_index != other.m_index; 
		}
	private:
		T*	m_buffer;
		int m_index;
	};

public:
	XArray(int bufferSize = 0) :m_buffer(nullptr), m_size(0) 
	{ 
		m_buffer = new T[bufferSize]; 
		m_bufferSize = bufferSize;
	}
	~XArray() { delete[] m_buffer; }

	void append(const T& value)
	{
		if ( m_bufferSize<m_size + 1)
		{
			int newBufferSize = m_size+1;
			T* newBuffer = new T[newBufferSize];
			memcpy(newBuffer, m_buffer, sizeof(T)*m_size);
			delete[] m_buffer;
			m_buffer = newBuffer;
			m_bufferSize = newBufferSize;
		}
		m_buffer[m_size] = value;
		++m_size;
	}
	void removeLast()
	{
		if (m_size > 0) {
			--m_size;
		}
	}
	T last()const
	{
		assert(m_size > 0);
		return m_buffer[m_size - 1];
	}
	
	iterator begin()const { return iterator(m_buffer,0); }
	iterator end()const { return iterator(m_buffer,m_size); }

	bool isEmpty()const { return (m_size==0); }

private:
	T*	m_buffer;
	int m_bufferSize;
	int m_size;
};

#endif // X_ARRAY_HPP
