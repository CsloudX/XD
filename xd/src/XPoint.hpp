#ifndef X_POINT_HPP
#define X_POINT_HPP

#include <vector>
#include <cassert>
#include "XVector.hpp"

class XPointN
{
public:
	virtual ~XPointN() {}
	XPointN() {}
	XPointN(int dimension) 
	{ 
		assert(dimension > 0); 
		m_data.resize(dimension);
	}
	XPointN(int dimension, double value)
	{
		assert(dimension > 0);
		m_data.resize(dimension, value);
	}
	int dimension()const { return m_data.size(); }

	const std::vector<double>& data()const { return m_data; }

	friend bool operator==(const XPointN &v1, const XPointN &v2)
	{
		assert(v1.dimension() == v2.dimension());
		return v1.m_data == v2.m_data;
	}
	friend bool operator!=(const XPointN &v1, const XPointN &v2)
	{
		assert(v1.dimension() == v2.dimension());
		return v1.m_data != v2.m_data;
	}

	friend const XPointN operator+(const XPointN &point, const XVectorN& vector)
	{
		assert(point.dimension() == vector.dimension());

		XPointN rs(point);
		int count = point.dimension();
		for (int i = 0; i < count; ++i) {
			rs[i] += vector.at(i);
		}
		return rs;
	}
	friend const XPointN operator-(const XPointN &point, const XVectorN& vector)
	{
		assert(point.dimension() == vector.dimension());

		XPointN rs(point);
		int count = point.dimension();
		for (int i = 0; i < count; ++i) {
			rs[i] -= vector.at(i);
		}
		return rs;
	}
	friend const XVectorN operator-(const XPointN &p1, const XPointN &p2)
	{
		assert(p1.dimension() == p2.dimension());

		int count = p1.dimension();
		XVectorN rs(count);
		for (int i = 0; i < count; ++i) {
			rs[i] = p1.at(i) - p2.at(i);
		}
		return rs;
	}

	friend const XPointN operator*(const XPointN &point, float factor)
	{
		XPointN rs(point);
		for (int i = 0; i < point.dimension(); ++i) {
			rs[i] *= factor;
		}
		return rs;
	}
	friend const XPointN operator*(float factor, const XPointN &point)
	{
		return point*factor;
	}


	friend const XPointN operator-(const XPointN &point)
	{
		XPointN rs(point);
		for (int i = 0; i < point.dimension(); ++i) {
			rs[i] = -point.at(i);
		}
		return rs;
	}
	friend const XPointN operator/(const XPointN &point, float divisor)
	{
		assert(divisor != 0);

		XPointN rs(point);
		for (int i = 0; i < point.dimension(); ++i) {
			rs[i] /= divisor;
		}
		return rs;
	}


	XPointN &operator*=(float factor)
	{
		*this = (*this)*factor;
		return *this;
	}


	XPointN &operator/=(float divisor)
	{
		*this = (*this) / divisor;
		return *this;
	}

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

protected:
	std::vector<double> m_data;
};

class XPoint2 : public XPointN
{
public:
	XPoint2(double x = 0, double y = 0)
		:XPointN(2) {
		m_data[0] = x;
		m_data[1] = y;
	}
	XPoint2(const XPointN& other)
	{
		m_data.resize(2);
		assert(other.dimension() == this->dimension());
		m_data = other.data();
	}

	void setX(double x) { m_data[0] = x; }
	void setY(double y) { m_data[1] = y; }

	double x()const { return m_data.at(0); }
	double y()const { return m_data.at(1); }
};

class XPoint3 : public XPointN
{
public:
	XPoint3(double x = 0, double y = 0, double z = 0)
		:XPointN(3) {
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
	}
	XPoint3(const XPointN& other)
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
};

class XPoint4 : public XPointN
{
public:
	XPoint4(double x = 0, double y = 0, double z = 0, double w=0)
		:XPointN(4) {
		m_data[0] = x;
		m_data[1] = y;
		m_data[2] = z;
		m_data[3] = w;
	}
	XPoint4(const XPointN& other)
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
};

#endif //X_POINT_HPP
