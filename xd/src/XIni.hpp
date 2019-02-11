#ifndef _X_INI_HPP
#define _X_INI_HPP

#include <fstream>
#include <XString.hpp>
#include <XVariant.hpp>
#include <unordered_map>

class XIni
{
public:
	typedef struct {
		XString key;
		XVariant value;
		XString comment;
	}ItemItem;

	typedef struct  {
		XString groupName;
		XString comment;
		std::unordered_map<std::wstring, ItemItem> items;
	}Item;

public:
	XIni(const XString& pathfile) :m_pathfile(pathfile) { load(pathfile); }
	~XIni()
	{
		if (m_needSave)
			save();
	}
	
	void setGroup(const XString& groupName)
	{
		auto itr = m_items.find(groupName);
		if (itr == m_items.end())
		{
			m_items[groupName].groupName = groupName;
		}
		m_currentGroupName = groupName;
	}
	void setValue(const XString& key, const XVariant& value)
	{
		if (m_items[m_currentGroupName].items[key].value != value)
		{
			m_items[m_currentGroupName].items[key].key = key;
			m_items[m_currentGroupName].items[key].value = value;
			m_needSave = true;
		}
	}
	XVariant value(const XString& key, const XVariant& defaultValue = XVariant())
	{
		auto itr = m_items[m_currentGroupName].items.find(key);
		if (itr != m_items[m_currentGroupName].items.end())
			return itr->second.value;
		else
			return defaultValue;
	}

private:
	bool load(const XString& pathfile);
	bool save()
	{
		std::wofstream fout(m_pathfile);
		if (!fout)
			return false;

		for (auto i = m_items.begin(); i != m_items.end(); ++i)
		{
			auto d = i->second;
			fout << L"[" << d.groupName << L"]";
			if (!d.comment.isEmpty())
				fout << L" ;" << d.comment;
			fout << XChar::newLine();

			for (auto j = d.items.begin(); j != d.items.end(); ++j)
			{
				auto item = j->second;
				fout << item.key << L" = " << item.value.toString();
				if (!item.comment.isEmpty())
					fout << L" ;" << item.comment;
				fout << XChar::newLine();
			}
		}
		m_needSave = false;
		return true;
	}

private:
	friend class XIniParser;
	XString m_pathfile;
	std::unordered_map<std::wstring, Item> m_items;
	std::wstring m_currentGroupName;
	bool m_needSave;
};

class XIniParser
{
public:
	class State
	{
	public:
		void parse(XIniParser* parser, wchar_t ch)
		{
			if (ch == L'\n')
			{
				if (!parser->m_groupName.isEmpty()){
					parser->appendGroup();
				}
				else if (!parser->m_key.isEmpty()){
					parser->appendItem();
				}
				changeState(parser, new NullState());
			}
			else {
				parseEvent(parser, ch);
			}
		}

	protected:
		void changeState(XIniParser* parser, State* state)
		{ parser->changeState(state); }
		
		virtual void parseEvent(XIniParser* parser, wchar_t ch) = 0;
	};
	class NullState :public State {
	protected:
		virtual void parseEvent(XIniParser* parser, wchar_t ch)
		{
			if (XChar(ch).isSpace())
				return;

			if (ch == L'[') {
				changeState(parser, new GroupNameState());
			}
			else{
				parser->m_key.append(ch);
				changeState(parser, new KeyState());
			}
		}
	};
	class GroupNameState :public State {
	protected:
		virtual void parseEvent(XIniParser* parser, wchar_t ch)
		{
			if (ch == L']') {
				changeState(parser, new GroupNameDoneState());
			}
			else{
				parser->m_groupName.append(ch);
			}
		}
	};
	class GroupNameDoneState :public State {
	protected:
		virtual void parseEvent(XIniParser* parser, wchar_t ch)
		{
			if (ch == L';') {
				changeState(parser, new CommentState());
			}
		}
	};
	class CommentState :public State {
	protected:
		virtual void parseEvent(XIniParser* parser, wchar_t ch)
		{
			parser->m_comment.append(ch);
		}
	};
	class KeyState :public State {
	protected:
		virtual void parseEvent(XIniParser* parser, wchar_t ch)
		{
			if (ch == L'=') {
				changeState(parser, new ValueState());
			}
			else {
				parser->m_key.append(ch);
			}
		}
	};
	class ValueState :public State {
	protected:
		virtual void parseEvent(XIniParser* parser, wchar_t ch)
		{
			if (ch == L';') {
				changeState(parser, new CommentState());
			}
			else {
				parser->m_value.append(ch);
			}
		}
	};

	XIniParser(XIni* ini) :m_ini(ini),m_state(new NullState()) {}
	~XIniParser() { X_DELETE(m_state); }

	void parse(wchar_t ch)
	{
		m_state->parse(this, ch);
	}

private:
	friend class State;
	void changeState(State* state)
	{
		X_DELETE(m_state);
		m_state = state;
	}
	void appendGroup()
	{
		m_ini->setGroup(m_groupName.trimmed());
		m_ini->m_items[m_ini->m_currentGroupName].comment = m_comment.trimmed();
		m_groupName.clear();
		m_comment.clear();
	}
	void appendItem()
	{
		auto key = m_key.trimmed();
		m_ini->m_items[m_ini->m_currentGroupName].items[key].key = m_key.trimmed();
		m_ini->m_items[m_ini->m_currentGroupName].items[key].value = m_value.trimmed();
		m_ini->m_items[m_ini->m_currentGroupName].items[key].comment = m_comment.trimmed();
		m_key.clear();
		m_value.clear();
		m_comment.clear();
	}
private:
	XIni* m_ini;
	State* m_state;
	XString m_groupName;
	XString m_comment;
	XString m_key;
	XString m_value;
};


bool XIni::load(const XString& pathfile)
{
	std::wifstream fin(pathfile);
	if (!fin.is_open())
		return false;

	XIniParser parser(this);
	wchar_t ch;
	while (fin.get(ch))
	{
		parser.parse(ch);
	}

	m_needSave = false;
	return true;
}

#endif _X_INI_HPP
