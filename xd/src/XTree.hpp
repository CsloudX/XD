#ifndef _X_TREE_HPP
#define _X_TREE_HPP

#include "XList.hpp"
#include "XNamespace.h"

template <typename _Ty>
class XTreeNode 
{
public:
	XTreeNode(const _Ty& data = _Ty(), XTreeNode* parent = nullptr)
		:m_data(data), m_parent(nullptr) 
	{
		link(parent, this);
	}
	virtual ~XTreeNode() { unlink(m_parent, this); }

	const _Ty& data()const { return m_data; }
	XTreeNode* parent()const { return m_parent; }
	const XList<XTreeNode*>& children()const { return m_children; }

	void setParent(XTreeNode* parent) { link(parent, this); }
	void appendChild(XTreeNode* child)  {  link(this, child); }
	void removeChild(XTreeNode* child) { unlink(this, child); }


	template<typename _Func>
	XTreeNode* findChild(_Func func, Xd::FindChildOption option=Xd::FindChildrenRecursively)const
	{
		switch (option)
		{
		case Xd::FindDirectChildrenOnly:
			for (auto& child : m_children){
				if (func(child))
					return child;
			}
			break;
		case Xd::FindChildrenRecursively:
			for (auto& child : m_children) {
				if (func(child))
					return child;

				auto ret = child->findChild(func, Xd::FindChildrenRecursively);
				if (ret)
					return ret;
			}
			break;
		default:
			break;
		}
		return nullptr;
	}
	template<typename _Func>
	XList<XTreeNode*> findChildren(_Func func, Xd::FindChildOption option = Xd::FindChildrenRecursively)const
	{
		XList<XTreeNode*> ret;
		switch (option)
		{
		case Xd::FindDirectChildrenOnly:
			for (auto& child : m_children) {
				if (func(child))
					ret.append(child);
			}
			break;
		case Xd::FindChildrenRecursively:
			for (auto& child : m_children) {
				if (func(child))
					ret.append(child);

				ret.append(child->findChildren(func, Xd::FindChildrenRecursively));
			}
			break;
		default:
			break;
		}
		return ret;
	}

private:
	static void link(XTreeNode* parent, XTreeNode* child)
	{
		if (parent == nullptr || child == nullptr)
			return;

		unlink(child->m_parent, child);
		child->m_parent = parent;
		parent->m_children.append(child);
	}
	static void unlink(XTreeNode* parent, XTreeNode* child)
	{
		if (parent == nullptr || child == nullptr)
			return;

		assert(child->m_parent == parent);
		parent->m_children.remove(child);
		child->m_parent = nullptr;
	}

protected:
	_Ty			m_data;
	XTreeNode*	m_parent;
	XList<XTreeNode*> m_children;
};

//template <typename T>
//class XTreeNode{
//public:
//	XTreeNode(){}
//	void setData(const T& data){ _data = data; } 
//	const T& data()const{ return _data; }
//	T& data(){ return _data; };
//
//	const XList<XTreeNode>& children()const{ return _children; }
//	int childCount()const{ return _children.size(); }
//	const XTreeNode& childAt(int idx)const{ 
//		assert(0 <= idx&& idx <= (int)_children.size());
//		return _children.at(idx); 
//	}
//	XTreeNode& childAt(int idx)
//	{
//		assert(0 <= idx&& idx <= (int)_children.size());
//		return _children[idx];
//	}
//
//	void addChild(const XTreeNode& child){ 
//        _children.append(child); 
//    }
//	void removeChild(const XTreeNode& child){ _children.remove(child); }
//	void removeChildAt(int i){ _children.removeAt(i); }
//	void removeAllChild(){ _children.clear(); }
//
//	bool operator==(const XTreeNode<T>& other)const{
//		return ((_data == other._data) && (_children == other._children));
//	}
//
//protected:
//    void setParent(XTreeNode* parent){ _parent = parent; }
//
//protected:
//	T	_data;
//	XList<XTreeNode> _children;
//};
//
//template <typename T>
//class XTree :public XTreeNode<T>{
//
//};



#endif //_X_TREE_HPP

