#ifndef _X_GRID_LAYOUT_HPP
#define _X_GRID_LAYOUT_HPP

#include <unordered_map>
#include "XLayout.hpp"
#include "XWidget.hpp"
#include "XFrame.hpp"

class XGridLayout : public XLayout{
private:
	typedef struct{
		XWidget* widget;
		int row;
		int col;
		int rowSpan;
		int colSpan;
	}Item;
public:
	XGridLayout(XWidget* parent=nullptr) :  _rowCount(0), _colCount(0)
		,_horizontalSpacing(6),_verticalSpacing(6){
		setMargins(10);
		if (parent)
			parent->setLayout(this);
	}

	void addWidget(XWidget* widget, int row, int col, int rowSpan = 1, int colSpan = 1){
		_items.append(Item{ widget, row, col, rowSpan, colSpan });
		if (row + rowSpan > _rowCount){
			_rowCount = row + rowSpan;
		}
		if (col + colSpan > _colCount){
			_colCount = col + colSpan;
		}
	}
	void addLayout(XLayout* layout, int row, int col, int rowSpan = 1, int colSpan = 1){
		XWidget* parent = static_cast<XWidget*>(_parent);
		assert(parent);
		XFrame* frame = new XFrame(parent);
		frame->setLayout(layout);
		addWidget(frame, row, col, rowSpan,colSpan);
	}
	void setHorizontalSpacing(int spacing){ _horizontalSpacing = spacing; }
	void setVerticalSpacing(int spacing){ _verticalSpacing = spacing; }
	void setSpacing(int spacing){ _horizontalSpacing = spacing; _verticalSpacing = spacing; }
	int horizontalSpacing()const{ return _horizontalSpacing; }
	int verticalSpacing()const{ return _verticalSpacing; }
	int spacing()const{
		if (_horizontalSpacing == _verticalSpacing)
			return _horizontalSpacing;
		return -1;
	}

	void setHorizontalStretch(int index, int stretch){ _horizontalStretch[index] = stretch; }
	void setVerticalStretch(int index, int stretch){ _verticalStretch[index] = stretch; }
	int horizontalStretch(int index)const{ return stretch(_horizontalStretch, index); }
	int verticalStretch(int index)const{ return stretch(_verticalStretch, index); }
	XWidget* parent(){ return static_cast<XWidget*>(_parent); }

protected:
	void update(const XSize& size){
		int halfHSpacing = _horizontalSpacing / 2;
		int halfVSpacing = _verticalSpacing / 2;
		XRect rect = XRect(0, 0, size.width(), size.height()).marginsRemoved(_margins)
			.marginsAdded(XMargins(halfHSpacing, halfVSpacing, halfHSpacing, halfVSpacing));

		auto xs = poss(_horizontalStretch, _colCount, rect.left(), rect.width());
		auto ys = poss(_verticalStretch, _rowCount, rect.top(), rect.height());

		//HDWP hDwp = ::BeginDeferWindowPos(_items.count());
		for (auto& var : _items){
			int left = xs.at(var.col)+halfHSpacing;
			int top = ys.at(var.row)+halfVSpacing;
			int right = xs.at(var.col + var.colSpan) - halfHSpacing;
			int bottom = ys.at(var.row + var.rowSpan) - halfVSpacing;
			var.widget->setRect(left, top, right - left, bottom - top);
			//hDwp = ::DeferWindowPos(hDwp, var.widget->hWnd(), 0, left, top, right-left, bottom-top, SWP_NOZORDER);
		}
		//::EndDeferWindowPos(hDwp);
	}

private:
	XList<int> poss(const std::unordered_map<int, int>& stretchs, int count, int startPos, int len){
		XList<int> retval;
		int stretchTotal = 0;
		for (int i = 0; i < count; ++i)
			stretchTotal += stretch(stretchs, i);
		int stretchFac = 0;
		retval.append(startPos);
		for (int i = 0; i < count; ++i){
			stretchFac += stretch(stretchs, i);
			retval.append(startPos + len*stretchFac / stretchTotal);
		}
		return retval;
	}
	int stretch(const std::unordered_map<int, int>& map, int index)const{
		auto itr = map.find(index);
		if (itr == map.end()){
			return 1;
		}
		return itr->second;
	}
	

protected:
	XList<Item> _items;
	int _rowCount;
	int _colCount;
	int _horizontalSpacing;
	int _verticalSpacing;
	std::unordered_map<int, int>	_horizontalStretch;
	std::unordered_map<int, int>	_verticalStretch;
};

#endif //_X_GRID_LAYOUT_HPP
