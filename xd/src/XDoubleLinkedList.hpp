#ifndef X_DOUBLE_LINKED_LIST_HPP
#define X_DOUBLE_LINKED_LIST_HPP

#include <cassert>

template<typename T>
class XDoubleLinkedList
{
private:
	struct Node {
		T value; Node* prev; Node* next;
		Node(const T& _value) :value(_value),prev(nullptr), next(nullptr) {}
	};

public:
	class iterator {
	public:
		iterator(Node* p) :m_ptr(p) {}
		T operator*()const { return m_ptr->value; }
		iterator& operator++() { m_ptr = m_ptr->next; return *this; }
		bool operator==(const iterator& other) { return this->m_ptr == other.m_ptr; }
		bool operator!=(const iterator& other) { return this->m_ptr != other.m_ptr; }
	private:
		Node* m_ptr;
	};

public:
	XDoubleLinkedList() :m_head(nullptr), m_tail(nullptr) {}
	~XDoubleLinkedList() { clear(); }
	void clear()
	{
		Node* node = m_head;
		while (node)
		{
			Node* p = node->next;
			delete node;
			node = p;
		}
	}

	void append(const T& value)
	{
		Node* node = new Node(value);
		if (m_tail != nullptr)
		{
			insertNode(node, m_tail, nullptr);
			m_tail = node;
		}
		else
		{
			m_head = m_tail = node;
		}
	}
	void prepend(const T& value)
	{
		Node* node = new Node(value);
		if (m_head != nullptr)
		{
			insertNode(node, nullptr, m_head);
			m_head = node;
		}
		else
		{
			m_head = m_tail = node;
		}
	}
	void removeFirst()
	{
		if (m_head)
		{
			if (m_head != m_tail)
			{
				Node* n = m_head->next;
				removeNode(m_head);
				m_head = n;
			}
			else
			{
				delete m_head;
				m_head = m_tail = nullptr;
			}
		}
	}
	void removeLast()
	{
		if (m_tail)
		{
			if (m_head != m_tail)
			{
				Node* n = m_tail->prev;
				removeNode(m_tail);
				m_tail = n;
			}
			else
			{
				delete m_tail;
				m_head = m_tail = nullptr;
			}
		}
	}

	iterator begin()const { return iterator(m_head); }
	iterator end()const { return iterator(nullptr); }

	bool isEmpty()const { return (m_head == nullptr); }
	T first()const { assert(m_head); return m_head->value; }
	T last()const { assert(m_tail); return m_tail->value; }

	bool contains(const T& value)const
	{
		for (auto v : (*this))
		{
			if (v == value)
				return true;
		}
		return false;
	}

private:
	static void insertNode(Node* node, Node* prev, Node* next)
	{
		assert(node);

		node->prev = prev;
		node->next = next;
		if (prev)
			prev->next = node;
		if (next)
			next->prev = node;
	}
	static void removeNode(Node* node)
	{
		assert(node);

		Node* prev = node->prev;
		Node* next = node->next;
		delete node;
		if (prev)
			prev->next = next;
		if (next) {
			next->prev = prev;
		}
	}

private:
	Node* m_head;
	Node* m_tail;
};

#endif // X_DOUBLE_LINKED_LIST_HPP
