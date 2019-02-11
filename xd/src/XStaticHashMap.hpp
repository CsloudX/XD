#ifndef _X_STATIC_HASH_MAP_HPP
#define _X_STATIC_HASH_MAP_HPP

#include <unordered_map>
#include <mutex>
#include <XMutexLocker.hpp>

template <typename _Key, typename _Value>
class XStaticHashMap;
template <typename _ID, typename _Cls>
class XHashItem
{
protected:
	virtual void setup(void* param=nullptr) {}

public:
	static XHashItem* getInstance(const _ID& id, void* param=nullptr)
	{
		if (!hashMap()->contains(id)) {
			auto item = new _Cls(id);
			item->setup(param);
		}
		return hashMap()->get(id);
	}

	virtual ~XHashItem()
	{
		if (m_haveMap)
			hashMap()->remove(m_id);
	}

	void clear()
	{
		delete this;
	}

protected:
	XHashItem(const _ID& id):m_id(id) 
	{
		m_haveMap = true;
		hashMap()->set(m_id, this);
	}

	const _ID& id()const { return m_id; }

private:
	static XStaticHashMap<_ID, XHashItem<_ID,_Cls>*>* hashMap();

private:
	friend class XStaticHashMap<_ID, XHashItem<_ID,_Cls>*>;
	_ID		m_id;
	bool	m_haveMap;
};

template <typename _Key, typename _Value>
class XStaticHashMap
{
public:
	~XStaticHashMap()
	{
		for (auto& var : m_hashMap)
		{
			var.second->m_haveMap = false;
		}
	}
	void set(const _Key& key, _Value value) 
	{ 
		XMutexLocker locker(&m_mtx);
		m_hashMap[key] = value; 
	}
	_Value get(const _Key&  key) { return m_hashMap[key]; }
	void remove(const _Key&  key)
	{
		XMutexLocker locker(&m_mtx);
		m_hashMap.erase(key); 
	}
	bool contains(const _Key&  key)
	{
		return (m_hashMap.find(key) != m_hashMap.end());
	}

private:
	std::unordered_map<_Key, _Value> m_hashMap;
	std::mutex m_mtx;
};

template <typename _ID, typename _Cls>
inline XStaticHashMap<_ID, XHashItem<_ID,_Cls>*>* XHashItem<_ID,_Cls>::hashMap()
{
	static XStaticHashMap<_ID, XHashItem*> obj;
	return &obj;
}



#endif // !_X_STATIC_HASH_MAP_HPP

