#ifndef X_VECTOR_HPP
#define X_VECTOR_HPP

#include "XMatrix.hpp"

class XVectorN:public XMatrix
{
public:
	virtual ~XVectorN(){}
	XVectorN(){}
	XVectorN(int dimension):XMatrix(1,dimension){ }
	XVectorN(int dimension, double value):XMatrix(1,dimension,value) { }
	XVectorN(const XMatrix& other)
	{
		assert(other.rowCount() == 1);
		*this = other;
	}
	int dimension()const { return m_data.size(); }
	double magnituide()const
	{
		double val = 0;
		for (double n : m_data) {
			val += (n*n);
		}
		return XMath::sqrt(val);
	}

	XVectorN normalized()const
	{
		double mag = magnituide();
		assert(mag != 0);

		if (mag == 1)
			return (*this);
		else
			return (*this) / magnituide();
	}
	void normalize()
	{
		*this = normalized();
	}

	double norm(int p)const
	{
		assert(p > 0);

		double total = 0;
		for (auto d : m_data)
		{
			total += XMath::pow(XMath::abs(d), p);
		}
		return XMath::pow(total, 1.0 / p);
	}
	double taxicabNorm()const { return norm(1); }
	double euclideanNorm()const { return norm(2); }

	double chebyshevNorm()const
	{
		assert(m_data.size() > 0);

		double ret = XMath::abs(m_data.at(0));
		int count = m_data.size();
		for (int i = 1; i < count; ++i)
		{
			ret = (XMath::max)(ret, XMath::abs(m_data.at(i)));
		}
		return ret;
	}
	double infinityNorm()const { return chebyshevNorm(); }

	//friend bool operator==(const XVectorN &v1, const XVectorN &v2)
	//{
	//	assert(v1.dimension() == v2.dimension());
	//	return v1.m_data == v2.m_data;
	//}
	//friend bool operator!=(const XVectorN &v1, const XVectorN &v2)
	//{
	//	assert(v1.dimension() == v2.dimension());
	//	return v1.m_data != v2.m_data;
	//}

	//friend const XVectorN operator+(const XVectorN &v1, const XVectorN &v2)
	//{
	//	assert(v1.dimension() == v2.dimension());
	//	
	//	XVectorN rs(v1);
	//	for (int i = 0; i < v1.dimension(); ++i) {
	//		rs[i] += v2.at(i);
	//	}
	//	return rs;
	//}

	//friend const XVectorN operator-(const XVectorN &v1, const XVectorN &v2)
	//{
	//	assert(v1.dimension() == v2.dimension());

	//	XVectorN rs(v1);
	//	for (int i = 0; i < v1.dimension(); ++i) {
	//		rs[i] -= v2.at(i);
	//	}
	//	return rs;
	//}

	//friend const XVectorN operator*(const XVectorN &vector, double factor)
	//{
	//	XVectorN rs(vector);
	//	for (int i = 0; i < vector.dimension(); ++i) {
	//		rs[i] *= factor;
	//	}
	//	return rs;
	//}
	//friend const XVectorN operator*(double factor, const XVectorN &vector)
	//{
	//	return vector*factor;
	//}
	//friend const XVectorN operator*(const XVectorN &v1, const XVectorN &v2)
	//{
	//	assert(v1.dimension() == v2.dimension());

	//	XVectorN rs(v1);
	//	for (int i = 0; i < v1.dimension(); ++i) {
	//		rs[i] *= v2.at(i);
	//	}
	//	return rs;
	//}
	//friend const XVectorN operator-(const XVectorN &vector)
	//{
	//	XVectorN rs(vector);
	//	for (int i = 0; i < vector.dimension(); ++i) {
	//		rs[i] = -vector.at(i);
	//	}
	//	return rs; 
	//}
	//friend const XVectorN operator/(const XVectorN &vector, float divisor)
	//{
	//	assert(divisor!=0);

	//	XVectorN rs(vector);
	//	for (int i = 0; i < vector.dimension(); ++i) {
	//		rs[i] /= divisor;
	//	}
	//	return rs;
	//}

	//friend const XVectorN operator/(const XVectorN &vector, const XVectorN &divisor)
	//{
	//	assert(vector.dimension() == divisor.dimension());

	//	XVectorN rs(vector);
	//	for (int i = 0; i < vector.dimension(); ++i) {
	//		assert(divisor.at(i) != 0);
	//		rs[i] /= divisor.at(i);
	//	}
	//	return rs;
	//}

	//XVectorN &operator+=(const XVectorN &vector)
	//{
	//	*this = (*this) + vector;
	//	return *this;
	//}

	//XVectorN &operator-=(const XVectorN &vector)
	//{
	//	*this = (*this) - vector;
	//	return *this;
	//}

	//XVectorN &operator*=(float factor)
	//{
	//	*this = (*this)*factor;
	//	return *this;
	//}

	//XVectorN &operator*=(const XVectorN &vector)
	//{
	//	*this = (*this)*vector;
	//	return *this;
	//}

	//XVectorN &operator/=(float divisor)
	//{
	//	*this = (*this) / divisor;
	//	return *this;
	//}

	//XVectorN &operator/=(const XVectorN &vector)
	//{
	//	*this = (*this) / vector;
	//	return *this;
	//}

	//static double dotProduct(const XVectorN &v1, const XVectorN &v2)
	//{
	//	assert(v1.dimension() == v2.dimension());

	//	int count = v1.dimension();
	//	double rs = 0;
	//	for (int i = 0; i < count; ++i) {
	//		rs += (v1.at(i)*v2.at(i));
	//	}
	//	return rs;
	//}

	double at(int index) const
	{
		assert(0 <= index && index < m_data.size());
		return m_data.at(index);
	}
	double& operator[](int index)
	{
		assert(0 <= index && index < m_data.size());
		return m_data[index];
	}
};

class XVector2 : public XVectorN
{
public:
	XVector2(double x = 0, double y = 0)
		:XVectorN(2) {
		m_data[0] = x;
		m_data[1] = y;
	}
	XVector2(const XVectorN& other)
	{
		m_data.resize(2);
		assert(other.dimension() == this->dimension());
		m_data = other.data();
	}

	void setX(double x) { m_data[0] = x; }
	void setY(double y) { m_data[1] = y; }

	double x()const { return m_data.at(0); }
	double y()const { return m_data.at(1); }

	static XVector2 zero() { return XVectorN(2, 0); }
	static XVector2 one() { return XVectorN(2, 1); }
};

class XVector3 : public XVectorN
{
public:
	XVector3(double x = 0, double y = 0,double z=0)
		:XVectorN(3) {
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
	}
	XVector3(const XVectorN& other)
	{
		m_data.resize(3);
		assert(other.dimension() == this->dimension());
		m_data = other.data();
	}

	void setX(double x) { m_data[0] = x; }
	void setY(double y) { m_data[1] = y; }
	void setZ(double z) { m_data[2] = z; }

	double x()const { return m_data.at(0); }
	double y()const { return m_data.at(1); }
	double z()const { return m_data.at(2); }

	void set(double x, double y, double z)
	{
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
	}

	static XVector3 zero() { return XVectorN(3, 0); }
	static XVector3 one() { return XVectorN(3, 1); }

	static XVector3 crossProduct(const XVector3 &v1, const XVector3 &v2)
	{
		return XVector3(
			v1.y()*v2.z() - v1.z()*v2.y(),
			v1.z()*v2.x()-v1.x()*v2.z(),
			v1.x()*v2.y()-v1.y()*v2.x()
			);
	}
};

class XVector4 : public XVectorN
{
public:
	XVector4(double x = 0, double y = 0, double z = 0,double w=0)
		:XVectorN(4) {
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
		m_data[3] = w;
	}
	XVector4(const XVector3& vec3,double w=0)
		:XVector4(4){
		m_data[0] = vec3.x();
		m_data[1] = vec3.y();
		m_data[2] = vec3.z();
		m_data[3] = w;
	}
	XVector4(const XVectorN& other)
	{
		m_data.resize(4);
		assert(other.dimension() == this->dimension());
		m_data = other.data();
	}

	void setX(double x) { m_data[0] = x; }
	void setY(double y) { m_data[1] = y; }
	void setZ(double z) { m_data[2] = z; }
	void setW(double w) { m_data[3] = w; }

	double x()const { return m_data.at(0); }
	double y()const { return m_data.at(1); }
	double z()const { return m_data.at(2); }
	double w()const { return m_data.at(3); }

	static XVector4 zero() { return XVectorN(4, 0); }
	static XVector4 one() { return XVectorN(4, 1); }
};


#endif //X_VECTOR_HPP
