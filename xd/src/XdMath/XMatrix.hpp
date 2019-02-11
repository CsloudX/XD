#ifndef _X_MATRIX_HPP
#define _X_MATRIX_HPP

#include <cassert>
#include <XGlobal.h>

#include <QDebug>
#include <QString>

template <typename _Ty>
class _XMatrix 
{
public:
    static void print(const _XMatrix& mat)
    {
        int rows = mat.rowCount();
        int cols = mat.columnCount();
        qDebug()<<"[";
        for(int row=0;row<rows;++row){
            QStringList ss;
            for(int col=0;col<cols;++col)
                ss.append(QString::number(mat.at(row,col)));
            qDebug()<<ss.join(" ");
        }
        qDebug()<<"]";
    }


        _XMatrix(int rowCount = 0, int columnCount = 0, _Ty* data = nullptr)
		:m_rowCount(0), m_columnCount(0), m_data(nullptr)
	{
		resize(rowCount, columnCount);
		if (data)
		{
			memcpy(m_data, data, rowCount*columnCount * sizeof(_Ty));
		}
	}
	virtual ~_XMatrix() { clear(); }

	_XMatrix(const _XMatrix& other)
	{
		m_rowCount = m_columnCount = 0;
		m_data = nullptr;
		clone(other, this);
	}
	_XMatrix& operator=(const _XMatrix& other)
	{
		clone(other, this);
		return *this;
	}

        int rowCount()const { return m_rowCount; }
        int columnCount()const { return m_columnCount; }
        const _Ty& at(int row,int column)const
	{
		assert(row < m_rowCount && column < m_columnCount);
		return m_data[row*m_columnCount + column];
	}
        _Ty& at(int row, int column)
	{
		assert(row < m_rowCount && column < m_columnCount);
		return m_data[row*m_columnCount + column];
	}

        void resize(int rowCount, int columnCount)
	{
            assert(rowCount>=0 && columnCount>=0);

            _Ty* data = new _Ty[rowCount*columnCount];
            assert(data);
            auto rows = xMin(m_rowCount, rowCount);
            auto cols = xMin(m_columnCount, m_columnCount);
            for (int row = 0; row < rows; ++row)
            {
                    memcpy(&data[row*m_columnCount], &m_data[row*m_columnCount], cols * sizeof(_Ty));
            }
            clear();

            m_rowCount = rowCount;
            m_columnCount = columnCount;
            m_data = data;
	}

	void clear()
	{
		if (m_data)
		{
			delete[] m_data;
			m_rowCount = 0;
			m_columnCount = 0;
		}
	}

	_Ty (max)()const
	{
		if (m_rowCount == 0 || m_columnCount == 0)
		{
			assert(false);
			return _Ty();
		}

		_Ty ret = at(0, 0);
                (foreachValue)([&](int row, int col,const _Ty& value) {
			if (value > ret)
				ret = value;
		});
		return ret;
	}
	_Ty(min)()const
	{
		if (m_rowCount == 0 || m_columnCount == 0)
		{
			assert(false);
			return _Ty();
		}

		_Ty ret = at(0, 0);
                (foreachValue)([&](int row, int col, const _Ty& value) {
			if (value < ret)
				ret = value;
		});
		return ret;
	}

	_XMatrix transposed() const
	{
		_XMatrix ret( m_columnCount, m_rowCount);
                (foreachValue)([&](int row, int col, const _Ty& value) {
			ret.at(col, row) = value;
		});
		return ret;
	}
	void transpose() { *this = transposed(); }

        _XMatrix row(int idx)const
	{
		if (idx >= m_rowCount)
		{
			assert(false);
			return _XMatrix();
		}

		_XMatrix ret(1, m_columnCount);
                for (int i = 0; i < m_columnCount; ++i)
			ret.at(0, i) = at(idx, i);

		return ret;
	}
        _XMatrix column(int idx)const
	{
		if (idx >= m_columnCount)
		{
			assert(false);
			return _XMatrix();
		}

		_XMatrix ret(m_rowCount, 1);
                for (int i = 0; i < m_rowCount; ++i)
			ret.at(i, 0) = at(i, idx);

		return ret;
	}
        void setRow(int idx,const _XMatrix& mat){
            if( (0<=idx&&idx<rowCount())
                &&mat.columnCount()==columnCount()
                && mat.rowCount()==1)
            {
                for(int i=0;i<mat.columnCount();++i)
                {
                    at(idx,i)=mat.at(0,i);
                }
            }else{
                assert(false);
            }
        }
        void setColumn(int idx,const _XMatrix& mat){
            if( (0<=idx&&idx<columnCount())
                &&mat.rowCount()==rowCount()
                && mat.columnCount()==1)
            {
                for(int i=0;i<mat.rowCount();++i)
                {
                    at(i,idx)=mat.at(i,0);
                }
            }else{
                assert(false);
            }
        }

        void swapRow(int row1, int row2){
            if((0<=row1&& row1<rowCount())
                &&(0<=row2&& row2<rowCount()))
            {
                auto mat = row(row1);
                setRow(row1,row(row2));
                setRow(row2,mat);
            }
            else{
                assert(false);
            }
        }
        void swapColumn(int col1, int col2){
            if((0<=col1&& col1<columnCount())
                &&(0<=col2&& col2<columnCount()))
            {
                auto mat = column(col1);
                setColumn(col1,column(col2));
                setColumn(col2,mat);
            }
            else{
                assert(false);
            }
        }

    void echelon(int startRow=0, int startColumn=0)
    {
        int row,col;
        if(!findPivotPos(&row,&col,startRow,startColumn))
            return;

        if(row!=startRow)
            swapRow(row,startRow);

        //print(*this);
        auto pivotRow = this->row(startRow);
        //print(pivotRow);

        for(row=startRow+1;row<m_rowCount;++row){
            auto tRow = this->row(row);
            auto tn = tRow.at(0,col);
            if(tn==0)
                continue;

            auto n = pivotRow.at(0,col);
            setRow(row,tRow*n-pivotRow*tn);
            //print(*this);
        }

        echelon(startRow+1,col);
    }

    // Reduced row echelon form
    void rref()
    {
        echelon();
        for(int row=)
    }



    bool findPivotPos(int* row,int* column, int startRow=0,int startColumn=0)const
    {
        for(int c=startColumn;c<m_columnCount;++c)
        {
            for(int r=startRow;r<m_rowCount;++r)
            {
                if(at(r,c)!=0)
                {
                    *row=r;
                    *column=c;
                    return true;
                }
            }
        }
        return false;
    }

	_XMatrix operator+()const
	{
		return *this;
	}
	_XMatrix operator-()const
	{
		_XMatrix ret(m_rowCount, m_columnCount);
                (foreachValue)([&] (int row,int col,const _Ty& value){
			ret.at(row, col) = -value;
		});
		return ret;
	}
	_XMatrix operator+(const _XMatrix& other)const
	{
		if (m_rowCount != other.m_rowCount || m_columnCount != other.m_columnCount)
		{
			assert(false);
			return _XMatrix();
		}

		_XMatrix ret(m_rowCount, m_columnCount);
                foreachValue([&](int row, int col, const _Ty& value) {
			ret.at(row, col) = value+other.at(row,col);
		});
		return ret;
	}
	_XMatrix operator-(const _XMatrix& other)const
	{
		return *this + (-other);
	}
	_XMatrix operator*(double d)const
	{
		_XMatrix ret(m_rowCount, m_columnCount);
                foreachValue([&](int row, int col, const _Ty& value) {
			ret.at(row, col) = value * d;
		});
		return ret;
	}
	_XMatrix operator/(double d)const
	{
		_XMatrix ret(m_rowCount, m_columnCount);
                foreachValue([&](int row, int col, const _Ty& value) {
			ret.at(row, col) = value / d;
		});
		return ret;
	}
	friend _XMatrix operator*(double d, const _XMatrix& mat)
	{
		return mat * 2;
	}

	_XMatrix operator*(const _XMatrix& other)const
	{
		if (m_columnCount != other.m_rowCount)
		{
			assert(false);
			return _XMatrix();
		}

		_XMatrix ret(m_rowCount, other.m_columnCount);
                ret.foreachElement([&](int row, int col) {
			_Ty value = 0;
                        for (int i = 0; i < m_columnCount; ++i){
				value += at(row, i)*other.at(i, col);
			}
			ret.at(row, col) = value;
		});
		return ret;
	}

	bool operator == (const _XMatrix& other)const 
	{
		if (m_rowCount != other.m_rowCount || m_columnCount != other.m_columnCount)
			return false;

                foreachValue([&](int row, int col, const _Ty& value) {
			if (value != other.at(row, col))
				return false;
		});
		return true;
	}
	bool operator != (const _XMatrix& other)const { return !(*this == other); }

	static _XMatrix gen(size_t rows, size_t cols, const _Ty& value)
	{
		_XMatrix ret(rows, cols);
		ret.foreachElement([&](size_t row, size_t col) {
			ret.at(row, col) = value;
		});
		return ret;
	}

private:


	static void clone(const _XMatrix& from, _XMatrix* to)
	{
		to->clear();
		to->m_rowCount = from.m_rowCount;
		to->m_columnCount = from.m_columnCount;
		size_t size = from.m_rowCount*from.m_columnCount;
		to->m_data = new _Ty[size];
		memcpy(to->m_data, from.m_data, size * sizeof(_Ty));
	}

	template <typename _Func>
	void foreachValue(_Func func) const
	{
                for (int row = 0; row < m_rowCount; ++row)
		{
                        for (int col = 0; col < m_columnCount; ++col)
			{
				func(row, col,at(row,col));
			}
		}
	}
	template <typename _Func>
	void foreachElement(_Func func) const
	{
                for (int row = 0; row < m_rowCount; ++row)
		{
                        for (int col = 0; col < m_columnCount; ++col)
			{
				func(row, col);
			}
		}
	}

private:
	_Ty*	m_data;
        int	m_rowCount;
        int	m_columnCount;
};

typedef _XMatrix<double> XMatrix;

#endif //_X_MATRIX_HPP
