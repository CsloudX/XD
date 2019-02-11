#pragma once

#include <QObject>
#include <QList>
#include <QPointF>
#include <QVector>
#include <QPainter>
#include <XVector.hpp>
#include <XTransform.hpp>

class X3DView;
class X3DObject : public QObject
{
	Q_OBJECT

public:
	X3DObject();
	X3DObject(X3DView* view);
	X3DObject(X3DObject* parent);
	virtual ~X3DObject();

	void addChild(X3DObject* child);
	void addSeries(const QList<XVector3>& series);
	void addSeries(const XVector3& p1, const XVector3& p2);

	void setPosition(double x, double y, double z) { m_position.set(x, y, z); updatePoints(); }
	void setRotation(double x, double y, double z) { m_rotation.set(x, y, z); updatePoints(); }
	void setScale(double x, double y, double z) { m_scale.set(x, y, z); updatePoints(); }

protected:
	void childEvent(QChildEvent *event);
	virtual void updateEvent(QPainter* painter);
	void updatePoints();

private:
	void init(X3DView* view, X3DObject* parent);
	int indexOfPoint(const XVector3& src);

private:
	friend class X3DView;
	class Point {  
	public:
		Point(){}
		Point(const XVector4& _src):src(_src){}
		XVector4 src; QPointF dst; 
	};
	QVector<Point>		m_points;
	QList<QList<int> >	m_series;

	X3DObject*	m_parent;
	X3DView*	m_view;
	QList<X3DObject*> m_children;

	XVector3 m_position;
	XVector3 m_rotation;
	XVector3 m_scale;
	XTransform m_transform;
};

class X3DCube :public X3DObject
{
	Q_OBJECT
public:
	X3DCube()
	{
		XVector3 p1(0, 0, 0);
		XVector3 p2(1, 0, 0);
		XVector3 p3(1, 1, 0);
		XVector3 p4(0, 1, 0);
		XVector3 p5(0, 0, 1);
		XVector3 p6(1, 0, 1);
		XVector3 p7(1, 1, 1);
		XVector3 p8(0, 1, 1);
		addSeries(p1, p2); addSeries(p2, p3); addSeries(p3, p4); addSeries(p4, p1);
		addSeries(p5, p6); addSeries(p6, p7); addSeries(p7, p8); addSeries(p8, p5);
		addSeries(p1, p5); addSeries(p2, p6); addSeries(p3, p7); addSeries(p4, p8);
	}

};
