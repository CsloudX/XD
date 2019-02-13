#ifndef _X_LAYOUT_HPP
#define _X_LAYOUT_HPP

#include "../XRect.hpp"

class XLayout:public XObject{
public:
	friend class XWidget;
	virtual ~XLayout(){}
	const XMargins& margins()const{ return _margins; }
	void setMargins(const XMargins& margins){ _margins = margins; }
	void setMargins(int left, int top, int right, int bottom){ _margins.set(left, top, right, bottom); }
	void setMargins(int margin){ _margins.set(margin, margin, margin, margin); }

protected:
	virtual void update(const XSize& size) = 0;


protected:
	XMargins	_margins;
};

#endif //_X_LAYOUT_HPP

