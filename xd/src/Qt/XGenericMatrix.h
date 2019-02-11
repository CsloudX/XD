#ifndef X_GENERIC_MATRIX_H
#define X_GENERIC_MATRIX_H

#include <QString>
#include <vector>
#include <cassert>

class XGenericMatrix
{
public:
	XGenericMatrix(int rowCount, int columnCount)
		:m_rowCount(rowCount), m_columnCount(columnCount)
	{
		assert(m_rowCount > 0 && m_columnCount > 0);
		m_data.resize(m_rowCount*m_columnCount);
	}
	XGenericMatrix(int rowCount, int columnCount, const std::initializer_list<QString>& data)
		:m_rowCount(rowCount), m_columnCount(columnCount)
	{
		assert(m_rowCount > 0 && m_columnCount > 0);
		assert(data.size() == m_rowCount*m_columnCount);
		m_data = data;
	}
	XGenericMatrix(int rowCount, int columnCount, const std::initializer_list<double>& data)
		:m_rowCount(rowCount), m_columnCount(columnCount)
	{
		assert(m_rowCount > 0 && m_columnCount > 0);
		assert(data.size() == m_rowCount*m_columnCount);

		m_data.resize(data.size());
		int i = 0;
		for (double d:data)
		{
			m_data[i] = QString::number(d);
			++i;
		}
	}
	int rowCount()const { return m_rowCount; }
	int columnCount()const { return m_columnCount; }

	const QString& at(int row, int column)const { return (*this)(row, column); }
	const QString& operator()(int row, int column) const
	{
		assert(0 <= row && row < m_rowCount && 0 <= column&& column < m_columnCount);
		return m_data.at(row*m_columnCount + column);
	}
	QString& operator()(int row, int column)
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
				if (row != col && (*this)(row, col)!="0")
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
				bool ok;
				double val = (*this)(row, col).toDouble(&ok);
				if (!ok) val = -1;

				if (row == col && val != 1)
					return false;
				if (row != col && val != 0)
					return false;
			}
		}
		return true;
	}

	XGenericMatrix transposed() const
	{
		XGenericMatrix ret(m_columnCount, m_rowCount);
		ret.foreachValueRef([=](int row, int col, QString& val) {
			val = (*this)(col, row);
		});
		return ret;
	}

	friend XGenericMatrix operator*(const XGenericMatrix &mat, const QString& factor)
	{
		XGenericMatrix rs(mat);
		rs.foreachValueRef([=](int row, int col, QString& val) {
			val = mul(val, factor);
		});
		return rs;
	}
	friend XGenericMatrix operator*(const QString& factor, const XGenericMatrix &mat)
	{
		return mat*factor;
	}
	friend XGenericMatrix operator*(const XGenericMatrix &mat1, const XGenericMatrix &mat2)
	{
		assert(mat1.columnCount() == mat2.rowCount());

		XGenericMatrix ret(mat1.rowCount(), mat2.columnCount());
		int count = mat1.columnCount();
		ret.foreachValueRef([=](int row, int col, QString& val) {
			val = "0";
			for (int k = 0; k < count; ++k)
			{
				val = add(val,mul( mat1(row, k),mat2(k, col)));
			}
		});

		return ret;
	}

	const std::vector<QString>& data()const { return m_data; }

private:
	template <typename Func>
	void foreachValue(Func func)const
	{
		for (int row = 0; row < m_rowCount; ++row)
		{
			for (int col = 0; col < m_columnCount; ++col)
			{
				func(row, col, (*this)(row, col));
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

	static QString add(const QString& s1, const QString& s2)
	{
		bool ok1, ok2;
		double d1 = s1.toDouble(&ok1);
		double d2 = s2.toDouble(&ok2);
		if (ok1&&ok2)
		{
			return QString::number(d1+d2);
		}
		else
		{
			if (ok1 && d1 == 0)
				return s2;
			if (ok2&& d2==0)
				return s1;

			return s1 + "+" + s2;
		}
	}
	static QString sub(const QString& s1, const QString& s2)
	{
		bool ok1, ok2;
		double d1 = s1.toDouble(&ok1);
		double d2 = s2.toDouble(&ok2);
		if (ok1&&ok2)
		{
			return QString::number(d1 - d2);
		}
		else
		{
			if (s1 == s2)
				return "0";
			else
				return s1 + "-" + s2;
		}
	}
	static QString mul(const QString& s1, const QString& s2)
	{
		bool ok1, ok2;
		double d1 = s1.toDouble(&ok1);
		double d2 = s2.toDouble(&ok2);

		if (ok1&&ok2)
		{
			return QString::number(d1 * d2);
		}
		else
		{
			if ((ok1 && d1 == 0) || (ok2&&d2 == 0))
				return "0";
			if (ok1 && d1 == 1)
				return s2;
			if (ok2&&d2 == 1)
				return s1;
			else
				return s1 + "*" + s2;
		}
	}
	static QString div(const QString& s1, const QString& s2)
	{
		bool ok1, ok2;
		double d1 = s1.toDouble(&ok1);
		double d2 = s2.toDouble(&ok2);
		if (ok1&&ok2)
		{
			assert(d2 != 0);
			return QString::number(d1 * d2);
		}
		else
		{
			if (ok1 && d1 == 0)
				return "0";
			else
				return s1 + "*" + s2;
		}
	}

protected:
	int m_rowCount;
	int m_columnCount;
	std::vector<QString> m_data;
};

#endif // X_GENERIC_MATRIX_H
