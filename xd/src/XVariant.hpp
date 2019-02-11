#ifndef _X_VARIANT_HPP
#define _X_VARIANT_HPP

#include "XString.hpp"

class XVariant
{
public:
	enum Type{Invalid,String,Int,Double,Bool};
	~XVariant() { X_DELETE(m_data); }

	class DataBase
	{
	private:
		DataBase(Type type) :m_type(type){}
		virtual ~DataBase() {}
		friend class XVariant;
		virtual XString toString()const { return XString(); }
		virtual int toInt()const { return 0; }
		virtual double toDouble()const { return 0.0; }
		virtual bool toBool()const { return false; }
		virtual bool isEqualEvent(const DataBase* other)const { return true; }
		virtual DataBase* clone()const = 0;
		bool isEqual(const DataBase* other)const
		{
			if (m_type != other->m_type)
				return false;
			return isEqualEvent(other);
		}
		Type	m_type;
	};

	template<typename T>
	class Data: public DataBase
	{
	public:
		Data(Type type,const T& data):DataBase(type),m_data(data){}
		virtual ~Data(){}
		T		m_data;
	};
	XVariant():m_data(nullptr){}

	class StringData :public Data<XString>
	{
	private:
		virtual XString toString()const { return m_data; }
		virtual int toInt()const { return m_data.toInt(); }
		virtual double toDouble()const { return m_data.toDouble(); }
		virtual bool toBool()const { return m_data.toBool(); }
		virtual bool isEqualEvent(const Data* other)const { return this->toString() == other->toString(); }
		virtual DataBase* clone()const { return new StringData(m_data); }
	public:
		StringData(const XString& str):Data(String,str){}
	};
	XVariant(const XString& str):m_data(new StringData(str)){}

	class IntData :public Data<int>
	{
	private:
		virtual XString toString()const { return XString::number(m_data); }
		virtual int toInt()const { return m_data; }
		virtual double toDouble()const { return m_data; }
		virtual bool toBool()const { return (m_data != 0); }
		virtual bool isEqualEvent(const Data* other)const { return this->toInt() == other->toInt(); }
		virtual DataBase* clone()const { return new IntData(m_data); }
	public:
		IntData(int n):Data(Int,n) {}
	};
	XVariant(int n) :m_data(new IntData(n)) {}

	class DoubleData :public Data<double>
	{
	private:
		virtual XString toString()const { return XString::number(m_data); }
		virtual int toInt()const { return (int)m_data; }
		virtual double toDouble()const { return m_data; }
		virtual bool toBool()const { return (m_data != 0); }
		virtual bool isEqualEvent(const Data* other)const { return this->toDouble() == other->toDouble(); }
		virtual DataBase* clone()const { return new DoubleData(m_data); }
	public:
		DoubleData(double d) :Data(Double,d) {}
	};
	XVariant(double d) :m_data(new DoubleData(d)) {}

	class BoolData :public Data<bool>
	{
	private:
		virtual XString toString()const { return XString::number(m_data); }
		virtual int toInt()const { return m_data ?1:0; }
		virtual double toDouble()const { return m_data ? 1 : 0; }
		virtual bool toBool()const { return m_data; }
		virtual bool isEqualEvent(const Data* other)const { return this->toBool() == other->toBool(); }
		virtual DataBase* clone()const { return new BoolData(m_data); }
	public:
		BoolData(bool b) :Data(Bool,b) {}
	};
	XVariant(bool b) :m_data(new BoolData(b)) {}

public:
	Type type()const { return m_data->m_type; }
	XString toString()const { return m_data->toString(); }
	int toInt()const { return m_data->toInt(); }
	double toDouble()const { return m_data->toDouble(); }
	bool toBool()const { return m_data->toBool(); }

	bool operator==(const XVariant &other) const
	{  return m_data->isEqual(other.m_data); }
	bool operator!=(const XVariant &other) const
	{ return !(m_data == other.m_data); }

	XVariant(const XVariant& other)
	{
		m_data = other.m_data->clone();
	}
	XVariant& operator=(const XVariant& other)
	{
		X_DELETE(m_data);
		m_data = other.m_data->clone();
		return *this;
	}


private:
	DataBase* m_data;
};



#endif //_X_VARIANT_HPP
