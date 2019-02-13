#ifndef _X_TABLE_VIEW_HPP
#define _X_TABLE_VIEW_HPP

#include "XWidget.hpp"


class XTableView : public XWidget{
public:
	enum SelectionBehavior{SelectItems,SelectRows,SelectColumns};
	XTableView(XWidget* parent = nullptr) 
		:XWidget(parent)
		, _defaultColumnWidth(80)
	{
		XWidget::createWindowEx("SysListView32", "",LVS_REPORT, parent);
	}
	void setDefaultColumnWidth(int width){ _defaultColumnWidth = width; }
	int defaultColumnWidth()const{ return _defaultColumnWidth; }
	int columnCount()const{
		int col = 0;
		_lvc.mask = LVCF_WIDTH;
		while(SendMessage(_hWnd, LVM_GETCOLUMN, col++, (LPARAM)&_lvc)==TRUE);
		return col-1;
	}
	void setColumnCount(int count){
		int col = columnCount();
		while (col < count){
			insertColumn(col++, XString());
		}
		while (col-- > count){
			removeColumn(col);
		}
	}
	void insertColumn(int column,const XString& text){
		assert(0<= column && column <= columnCount());
		_lvc.mask = LVCF_TEXT|LVCF_WIDTH;
		_lvc.pszText = (LPWSTR)text.c_str();
		_lvc.cx = _defaultColumnWidth;
		SendMessage(_hWnd, LVM_INSERTCOLUMN, column, (LPARAM)&_lvc);
	}
	void removeColumn(int column){
		assert(0<=column && column < columnCount());
		SendMessage(_hWnd, LVM_DELETECOLUMN, column, 0);
	}
	void setColumnText(int column, const XString& text){
		_lvc.mask = LVCF_TEXT;
		_lvc.pszText = (LPWSTR)text.c_str();
		SendMessage(_hWnd,LVM_SETCOLUMN, column, (LPARAM)&_lvc);
	}

	void setColumnWidth(int column, int width){
		ListView_SetColumnWidth(_hWnd, column, width);
	}
	int rowCount()const{
		int row = 0;
		_lvi.mask = 0;
		while (SendMessage(_hWnd, LVM_GETITEM, 0, (LPARAM)&_lvi)){
			++_lvi.iItem;
		}
		return _lvi.iItem;
	}
	void setRowCount(int count){
		int row = rowCount();
		while (row < count){
			insertRow(row++);
		}
		while (row-- > count){
			removeRow(row);
		}
	}
	void insertRow(int row){
		assert(0<=row && row <= rowCount());
		_lvi.mask = 0;
		_lvi.iItem = row;
		SendMessage(_hWnd, LVM_INSERTITEM, 0, (LPARAM)&_lvi);
	}
	void removeRow(int row){
		assert(0<=row && row < rowCount());

		_lvi.mask = 0;
		_lvi.iItem = row;
		SendMessage(_hWnd, LVM_DELETEITEM, 0, (LPARAM)&_lvi);
	}
	void setText(int row, int column, const XString& text){
		assert(0<=row && row < rowCount());
		assert(0<=column && column < columnCount());
		_lvi.mask = LVIF_TEXT;
		_lvi.iItem = row;
		_lvi.iSubItem = column;
		_lvi.pszText = (LPWSTR)text.c_str();
		SendMessage(_hWnd, LVM_SETITEM, 0, (LPARAM)&_lvi);
	}
	
	void setSelectionBehavior(SelectionBehavior behavior){
		UINT mask = LVS_EX_FULLROWSELECT | LVS_EX_SIMPLESELECT;
		UINT style = 0;
		switch (behavior)
		{
		case XTableView::SelectItems:style = LVS_EX_SIMPLESELECT; break;
		case XTableView::SelectRows: style = LVS_EX_FULLROWSELECT; break;
		case XTableView::SelectColumns: break;
		default: break;
		}

		SendMessage(_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, mask, style);
	}
	void setShowGrid(bool show){
		SendMessage(_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES, show ? LVS_EX_GRIDLINES:0);
	}

private:
	mutable LV_COLUMN	_lvc;
	mutable LV_ITEM		_lvi;
	int _defaultColumnWidth;
};

#endif //_X_TABLE_VIEW_HPP
