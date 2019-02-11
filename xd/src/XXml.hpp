#ifndef _X_XML_HPP
#define _X_XML_HPP

#include <fstream>
#include "XTree.hpp"
#include "XString.hpp"

class XXmlAttr
{
public:
	XString key;
	XString value;

	XXmlAttr(const XString& _key, const XString& _value)
		:key(_key), value(_value) {}
};

class XXmlData
{
public:
	XString key;
	XString value;
	XList<XXmlAttr> attrs;

	XXmlData(const XString& _key = XString(), const XString& _value = XString())
		:key(_key),value(_value){}

	void setAttr(const XString& key, const XString& value)
	{
		for (auto& attr : attrs)
		{
			if (attr.key == key)
			{
				attr.value = value;
				return;
			}
		}
		attrs.append(XXmlAttr(key, value));
	}
        void setAttr(const XString& key, int n){setAttr(key,XString::number(n));}
        void setAttr(const XString& key, double d){setAttr(key,XString::number(d));}
        void setAttr(const XString& key, bool b){setAttr(key,XString::fromBool(b));}

	XString attr(const XString& key)const
	{
		for (auto& attr : attrs)
		{
			if (attr.key == key)
			{
				return attr.value;
			}
		}
		return XString();
	}

private:
};

class XXmlNode : protected XTreeNode<XXmlData>
{
public:
	virtual ~XXmlNode()
	{
		clear();
	}

protected:
	XXmlNode(const XString& key = XString()) { m_data.key = key; }

public:
	XXmlNode* findChild(const XString& key)const
	{
		auto ret = XTreeNode::findChild([=](XTreeNode<XXmlData>* child) {
			return child->data().key == key;
		},Xd::FindDirectChildrenOnly);

		return static_cast<XXmlNode*>(ret);
			
	}
	XList<XXmlNode*> children()const
	{
		XList<XXmlNode*> ret;
		for (auto& child : m_children)
			ret.append(static_cast<XXmlNode*>(child));
		return ret;
	}
	const XXmlData& data()const { return m_data; }
	XXmlData& data() { return m_data; }
	void clear()
	{
		while (!m_children.isEmpty()) {
			delete m_children.at(0);
		}
	}
	bool fromString(const XString& str) 
	{ 
		clear();
		return parse(str.c_str()); 
	}
	XString toString(int tabCount=0)const {
		XString tagBegin;
		XString tagEnd;
		XString tagMid;

		assert(!m_data.key.empty());

		// tag begin
		tagBegin.append(L"<");
		tagBegin.append(escape( m_data.key,false));
		for (const auto& attr : m_data.attrs)
		{
			tagBegin.append(L" ");
			tagBegin.append(escape( attr.key,false));
			tagBegin.append(L"=\"");
			tagBegin.append(escape( attr.value,false));
			tagBegin.append(L"\"");
		}
		tagBegin.append(L">");

		// tag mid
		if (m_children.size() == 0) {
			tagMid.append(escape( m_data.value,false));
		}
		else {
			int count = m_children.count();
			tagMid.append(L"\n");
			for (int i = 0; i < count; i++)
			{
				XTreeNode<XXmlData>* pNode = m_children.at(i);
				XXmlNode* node = static_cast<XXmlNode*>(pNode);
				tagMid.append(node->toString(tabCount+1));
				tagMid.append(L"\n");
			}
		}

		tagEnd.append(L"</");
		tagEnd.append(m_data.key);
		tagEnd.append(L">");

		XString strTabs;
		for (int i = 0; i < tabCount; ++i)
			strTabs.append(L"\t");

		XString retval;
		retval.append(strTabs); 
		retval.append(tagBegin);

		retval.append(tagMid);

		if(tagMid.endWith(L"\n"))
			retval.append(strTabs);
		retval.append(tagEnd);

		return retval;
	}
	XXmlNode* createSubNode(const XString& key=XString())
	{
		XXmlNode* node = new XXmlNode(key);
		appendChild(node);
		return node;
	}

private:
	bool parse(const wchar_t* pstr)
	{
		m_pCur = pstr;
		if (!parseStartTag())   return false;

		if (!m_parsedEndTag) {
			while (haveChildNode())
			{
				XXmlNode* node = createSubNode();
				if (node->parse(m_pCur)) {
					m_pCur = node->m_pCur;
				}
				else {
					return false;
				}
			}

			if (!parseValue())  return false;
			if (!parseEndTag()) return false;
		}

		return true;
	}
	bool parseStartTag()
	{
		skipSpace();

		if (*m_pCur != L'<')     return false;
		++m_pCur;
		skipSpace();

		if (!parseTagName())    return false;

		while (parseAttr()) {}

		skipSpace();

		if (*m_pCur == L'/') {
			m_parsedEndTag = true;
			++m_pCur;
			skipSpace();
		}
		else {
			m_parsedEndTag = false;
		}
		if (*m_pCur != L'>') return false;
		++m_pCur;
		return true;
	}
	bool parseEndTag()
	{
		skipSpace();
		if (*m_pCur != L'<')     return false;
		++m_pCur;

		skipSpace();
		if (*m_pCur != L'/')     return false;
		++m_pCur;

		skipSpace();
		const wchar_t* pos = m_pCur;
		while (!XChar::isSpace(*pos) && (*pos != L'>')) {
			++pos;
		}
		XString tagName = escape(std::wstring(m_pCur, pos - m_pCur), true);
		if (m_data.key != tagName)   return false;
		m_pCur = pos;
		skipSpace();
		if (*m_pCur != L'>') return false;
		++m_pCur;
		return true;
	}
	void skipSpace()
	{
		while (XChar::isSpace(*m_pCur))
			++m_pCur;
	}
	bool parseTagName()
	{
		const wchar_t* pos = m_pCur;
		while (!XChar::isSpace(*pos) && (*pos != L'/') && (*pos != L'>'))
			++pos;
		m_data.key=escape( std::wstring(m_pCur, pos - m_pCur),true);
		m_pCur = pos;
		return (m_data.key.length() != 0);
	}

	bool parseAttr()
	{
		std::wstring attrName;
		std::wstring attrValue;

		skipSpace();

		// parse attr name
		const wchar_t* pos = m_pCur;
		while (!XChar::isSpace(*pos)
			&& (*pos != L'/')
			&& (*pos != L'>')
			&& (*pos != L'=')
			) {
			++pos;
		}
		attrName =escape( std::wstring(m_pCur, pos - m_pCur),true);
		m_pCur = pos;

		// parse =
		skipSpace();
		if (*m_pCur != L'=') return false;
		++m_pCur;
		skipSpace();

		// parse " vlaue "
		if ((*m_pCur != L'\'') && (*m_pCur != L'\"'))   return false;
		++m_pCur;
		pos = m_pCur;
		while ((*pos != L'\'') && (*pos != L'\"')) {
			++pos;
		}
		attrValue =escape( std::wstring(m_pCur, pos - m_pCur),true);
		m_pCur = pos + 1;

		m_data.setAttr(attrName, attrValue);
		return true;
	}
	bool haveChildNode()const
	{
		const wchar_t* pos = m_pCur;
		while (XChar::isSpace(*pos))
			++pos;
		if (*pos == L'<')
		{
			return (*(pos + 1) != L'/');
		}
		return false;
	}
	bool parseValue()
	{
		const wchar_t* pos = m_pCur;
		while (*pos != L'<') {
			if (*pos == L'\0')
				return false;
			++pos;
		}
		m_data.value = escape(std::wstring(m_pCur, pos - m_pCur),true);
		m_pCur = pos;
		return true;
	}

	static XString escape(const XString& str, bool back) 
	{
		struct {
			XString src; XString dst;
		}static dict[] = {
			L"&", L"&amp;",
			L"<", L"&lt;",
			L">", L"&gt;",
			L"\"", L"&quot;",
			L"\'", L"&apos;"
		};
		static const int dictCount = sizeof(dict) / sizeof(dict[0]);
	
		XString retval = str;
		if (!back) {
			for (size_t i = 0; i < dictCount; i++)
			{
				retval.replace(dict[i].src, dict[i].dst);
			}
		}
		else {
			for (size_t i = 0; i < dictCount; i++)
			{
				retval.replace(dict[i].dst, dict[i].src);
			}
		}
		return retval;
	}

private:
	const wchar_t* m_pCur;
	bool	m_parsedEndTag;
};

class XXml : public XXmlNode
{
public:
	XXml() {}
	bool load(const XString& fileName)
	{
		clear();

		std::wifstream fin(fileName);
		fin.imbue(std::locale(""));
		if (!fin)
			return false;

		std::wstring text((std::istreambuf_iterator<wchar_t>(fin)),
			std::istreambuf_iterator<wchar_t>());

		return fromString(text);
	}
	bool save(const XString& fileName) {
		std::wofstream fout(fileName);
		fout.imbue(std::locale(""));
		if (!fout)
			return false;

		fout << toString();
		return true;
	}
};

#endif //_X_XML_HPP
