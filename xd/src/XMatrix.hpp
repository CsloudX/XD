#ifndef X_MATRIX_HPP
#define X_MATRIX_HPP

#include <vector>
#include <cassert>
#include "XMath.hpp"
#include "XUnitTest.hpp"

class XMatrix
{
public:
	XMatrix() {}
	XMatrix(int rowCount, int columnCount)
		:m_rowCount(rowCount),m_columnCount(columnCount)
	{
		assert(m_rowCount > 0 && m_columnCount > 0);

		int count = m_rowCount*m_columnCount;
		m_data.resize(count);
	}
	XMatrix(int rowCount, int columnCount, double value)
		:m_rowCount(rowCount), m_columnCount(columnCount)
	{
		assert(m_rowCount > 0 && m_columnCount > 0);

		int count = m_rowCount*m_columnCount;
		m_data.resize(count,value);
	}
	XMatrix(int rowCount, int columnCount, const std::initializer_list<double>& data)
		:m_rowCount(rowCount), m_columnCount(columnCount)
	{
		assert(m_rowCount > 0 && m_columnCount > 0);

		int count = m_rowCount*m_columnCount;
		assert(data.size() == count);

		m_data = data;
	}
	int rowCount()const { return m_rowCount; }
	int columnCount()const { return m_columnCount; }

	const double& at(int row, int column)const{return (*this)(row, column);}
	const double& operator()(int row, int column) const 
	{
		assert(0 <= row && row < m_rowCount && 0 <= column&& column < m_columnCount);
		return m_data.at(row*m_columnCount + column);
	}
	double& operator()(int row, int column)
	{
		assert(0 <= row && row < m_rowCount && 0 <= column&& column < m_columnCount);
		return m_data[row*m_columnCount + column];
	}

	bool isSquareMatrix()const { return m_rowCount == m_columnCount; }
	bool isDiagonalMatrix()const
	{
		if (!isSquareMatrix())
			return false;

		for (int row = 0; row < m_rowCount; ++row)
		{
			for (int col = 0; col < m_columnCount; ++col)
			{
				if (row != col && (*this)(row, col) != 0)
					return false;
			}
		}
		return true;
	}
	bool isIdentityMatrix()const
	{
		if (!isSquareMatrix())
			return false;

		for (int row = 0; row < m_rowCount; ++row)
		{
			for (int col = 0; col < m_columnCount; ++col)
			{
				double val = (*this)(row, col);
				if (row == col && val != 1)
					return false;
				if (row != col && val != 0)
					return false;
			}
		}
		return true;
	}

	bool isSymmetricMatrix()const
	{
		return (*this) == transposed();
	}
	bool isSkewSymmetricMatrix()const
	{
		return transposed() == -(*this);
	}
	bool isAntisymmetricMatrix()const { return isSkewSymmetricMatrix(); }

	bool isOrthogonalMatrix()const
	{
		if (!isSquareMatrix())
			return false;

		return ((*this)*transposed()).isIdentityMatrix();
	}

	XMatrix transposed() const
	{
		XMatrix ret(m_columnCount, m_rowCount);
		ret.foreachValueRef([=](int row, int col, double& val) {
			val = (*this)(col, row);
		});
		return ret;
	}

	double determinant()const
	{
		assert(isSquareMatrix());

		int count = m_rowCount;
		if (count == 2){
			return at(0, 0)*at(1, 1) - at(0, 1)*at(1, 0);
		}

		double rs = 0;
		int i = 0;
		for (int j = 0; j < count; ++j)
		{
			rs += (at(i, j)*cofactor(i, j));
		}

		return rs;
	}
	double minor(int row, int col)const
	{
		assert(isSquareMatrix()
			&& 0 <= row && row < m_rowCount
			&& 0 <= col && col < m_columnCount);

		XMatrix mat(m_rowCount - 1, m_columnCount - 1);
		mat.foreachValueRef([=](int r, int c, double& val) {
			if (r >= row) ++r;
			if (c >= col) ++c;
			val = at(r, c);
		});
		return mat.determinant();
	}
	double cofactor(int row, int col)const
	{
		if ((row + col) % 2 == 0) // (-1)^(row+col)
		{
			return minor(row, col);
		}
		else
		{
			return minor(row, col)*-1;
		}
	}

	XMatrix classicalAdjoint()const
	{
		assert(isSquareMatrix());

		XMatrix mat(m_rowCount, m_columnCount);
		mat.foreachValueRef([=](int row, int col, double& val) {
			val = cofactor(col, row);
		});
		return mat;
	}

	XMatrix inverse()const
	{
		assert(isSquareMatrix());
		return classicalAdjoint() / determinant();
	}

	static XMatrix identityMatrix(int dim)
	{
		XMatrix mat(dim, dim, 0);
		for (int i = 0; i < dim; ++i)
		{
			mat(i, i) = 1;
		}
		return mat;
	}

	friend XMatrix operator*(const XMatrix &mat, double factor)
	{
		XMatrix rs(mat);
		rs.foreachValueRef([=](int row, int col, double& val) {
			val *= factor;
		});
		return rs;
	}
	friend XMatrix operator/(const XMatrix &mat, double factor)
	{
		assert(factor != 0);

		XMatrix rs(mat);
		rs.foreachValueRef([=](int row, int col, double& val) {
			val /= factor;
		});
		return rs;
	}
	friend XMatrix operator*( double factor, const XMatrix &mat)
	{
		return mat*factor;
	}
	friend XMatrix operator*(const XMatrix &mat1, const XMatrix &mat2)
	{
		assert(mat1.columnCount() == mat2.rowCount());

		XMatrix ret(mat1.rowCount(), mat2.columnCount());
		int count = mat1.columnCount();
		ret.foreachValueRef([=](int row, int col, double& val) {
			val = 0;
			for (int k = 0; k < count; ++k)
			{
				val += (mat1(row, k)*mat2(k, col));
			}
		});

		return ret;
	}

	friend XMatrix operator-(const XMatrix &mat)
	{
		XMatrix rs(mat);
		rs.foreachValueRef([=](int row, int col, double& val) {
			val *= -1;
		});
		return rs;
	}

	friend bool operator==(const XMatrix &m1, const XMatrix &m2)
	{
		return (m1.rowCount() == m2.rowCount()
			&& m1.columnCount() == m2.columnCount()
			&& m1.data() == m2.data());
	}
	friend bool operator!=(const XMatrix &m1, const XMatrix &m2)
	{
		return !(m1 == m2);
	}

	const std::vector<double>& data()const { return m_data; }

protected:
	template <typename Func>
	void foreachValue(Func func)const
	{
		for (int row = 0; row < m_rowCount; ++row)
		{
			for (int col = 0; col < m_columnCount; ++col)
			{
				func(row, col,(*this)(row,col));
			}
		}
	}
	template <typename Func>
	void foreachValueRef(Func func)
	{
		for (int row = 0; row < m_rowCount; ++row)
		{
			for (int col = 0; col < m_columnCount; ++col)
			{
				func(row, col, (*this)(row, col));
			}
		}
	}

protected:
	int m_rowCount;
	int m_columnCount;
	std::vector<double> m_data;
};

class XMatrix2x2 :public XMatrix
{
public:
	XMatrix2x2():XMatrix(2, 2){}
	XMatrix2x2(
		double m11,double m12,
		double m21,double m22
	) :XMatrix(2, 2)
	{
		(*this)(0, 0) = m11; (*this)(0, 1) = m12;
		(*this)(1, 0) = m21; (*this)(1, 1) = m22;
	}
	XMatrix2x2(const XMatrix& mat):XMatrix(2,2)
	{
		assert(mat.rowCount() == rowCount() && mat.columnCount() == columnCount());
		m_data = mat.data();
	}
};

class XMatrix3x3 :public XMatrix
{
public:
	XMatrix3x3() :XMatrix(3, 3) {}
	XMatrix3x3(
		double m11, double m12, double m13,
		double m21, double m22, double m23,
		double m31, double m32, double m33
	) :XMatrix(3, 3)
	{
		(*this)(0, 0) = m11; (*this)(0, 1) = m12; (*this)(0, 2) = m13;
		(*this)(1, 0) = m21; (*this)(1, 1) = m22; (*this)(1, 2) = m23;
		(*this)(2, 0) = m31; (*this)(2, 1) = m32; (*this)(2, 2) = m33;
	}
	XMatrix3x3(const XMatrix& mat) :XMatrix(3, 3)
	{
		assert(mat.rowCount() == rowCount() && mat.columnCount() == columnCount());
		m_data = mat.data();
	}
};

class XMatrix4x4 :public XMatrix
{
public:
	XMatrix4x4() :XMatrix(4, 4) {}
	XMatrix4x4(
		double m11, double m12, double m13, double m14,
		double m21, double m22, double m23,	double m24,
		double m31, double m32, double m33, double m34,
		double m41, double m42, double m43, double m44
	) :XMatrix(4, 4)
	{
		(*this)(0, 0) = m11; (*this)(0, 1) = m12; (*this)(0, 2) = m13; (*this)(0, 3) = m14;
		(*this)(1, 0) = m21; (*this)(1, 1) = m22; (*this)(1, 2) = m23; (*this)(1, 3) = m24;
		(*this)(2, 0) = m31; (*this)(2, 1) = m32; (*this)(2, 2) = m33; (*this)(2, 3) = m34;
		(*this)(3, 0) = m41; (*this)(3, 1) = m42; (*this)(3, 2) = m43; (*this)(3, 3) = m44;
	}
	XMatrix4x4(const XMatrix& mat) :XMatrix(4, 4)
	{
		assert(mat.rowCount() == rowCount() && mat.columnCount() == columnCount());
		m_data = mat.data();
	}
};

class XMatrixTest : public XUnitTest
{
protected:
	virtual void testEvent()
	{
		X_COMPARE(XMatrix2x2(2, 1, -1, 2).determinant(), 5);
		X_COMPARE(XMatrix2x2(-3, 4, 2, 5).determinant(), -23);

	 	X_OUTPUT( XMatrix3x3 mat( 
			-4, -3, 3, 
			0, 2, -2, 
			1, 4, -1));

		X_COMPARE(mat.minor(0,1), 2);
		X_COMPARE(mat.determinant(), -24);

		XMatrix3x3 adj(
			6, 9, 0,
			-2, 1, -8,
			-2, 13, -8);
		X_COMPARE(mat.classicalAdjoint(), XMatrix(adj));

		XMatrix3x3 inverse(
			-1.0 / 4, -3.0 / 8, 0,
			1.0 / 12, -1.0 / 24, 1.0 / 3,
			1.0 / 12, -13.0 / 24, 1.0 / 3
		);
		X_COMPARE(mat.inverse(), XMatrix(inverse));
	}
};

#endif // X_MATRIX_HPP
