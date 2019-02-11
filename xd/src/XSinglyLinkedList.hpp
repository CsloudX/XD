#ifndef X_SINGLY_LINKED_LIST_HPP
#define X_SINGLY_LINKED_LIST_HPP

template<typename T>
class XSinglyLinkedList
{
private:
	struct Node { 
		T value; Node* next; 
		Node(const T& _value):value(_value),next(nullptr){}
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
	XSinglyLinkedList():m_head(nullptr),m_tail(nullptr) {}
	~XSinglyLinkedList() {
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
			m_tail->next = node;
			m_tail = node;
		}
		else
		{
			m_head = m_tail = node;
		}
	}
	//void prepend(const T& value)
	//{
	//	Node* node = new Node(value);
	//	if (m_head != nullptr)
	//	{
	//		node->next = m_head->next;
	//		m_head = node;
	//	}
	//	else 
	//	{
	//		m_head = m_tail = node;
	//	}
	//}
	//void removeFirst()
	//{
	//	if (m_head)
	//	{
	//		if (m_head == m_tail)
	//		{
	//			delete m_head;
	//			m_head = m_tail = nullptr;
	//		}
	//		else
	//		{
	//			Node* tmp = m_head->next;
	//			delete m_head;
	//			m_head = tmp;
	//		}
	//	}
	//}
	//void removeLast()
	//{
	//	if (m_tail)
	//	{
	//		if (m_head == m_tail)
	//		{
	//			delete m_tail;
	//			m_head = m_tail = nullptr;
	//		}
	//		else
	//		{
	//			Node* tmp = m_head->next;
	//			delete m_head;
	//			m_head = tmp;
	//		}
	//	}
	//}

	iterator begin()const { return iterator(m_head); }
	iterator end()const { return iterator(nullptr); }

private:
	Node* m_head;
	Node* m_tail;


};

#endif // X_SINGLY_LINKED_LIST_HPP
