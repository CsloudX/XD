#include "X3DObject.h"
#include <QChildEvent>
#include "X3DView.h"

X3DObject::X3DObject()
{
	init(nullptr, nullptr);
}

X3DObject::X3DObject(X3DView* view)
{
	init(view, nullptr);
}

X3DObject::X3DObject(X3DObject* parent)
{
	init(nullptr, parent);
}

X3DObject::~X3DObject()
{
}

void X3DObject::addChild( X3DObject* child)
{
	child->QObject::setParent(this);
	m_children.append(child);
}

void X3DObject::addSeries(const QList<XVector3>& series)
{
	QList<int> series2;
	for (const XVector3& src : series)
		series2.append(indexOfPoint(src));
	m_series.append(series2);
	updatePoints();
}

void X3DObject::addSeries(const XVector3& p1, const XVector3& p2)
{
	QList<XVector3> series;
	series << p1 << p2;
	addSeries(series);
}


void X3DObject::childEvent(QChildEvent *event)
{
	if (event->removed())
		m_children.removeOne(qobject_cast<X3DObject*>( event->child()));
}

void X3DObject::updateEvent(QPainter* painter)
{
	for (const auto& series : m_series)
	{
		QPolygonF polygon;
		for (int idx : series)
		{
			polygon.append(m_points.at(idx).dst);
		}
		painter->drawPolyline(polygon);
	}
}

void X3DObject::updatePoints()
{
	m_transform.reset();
	m_transform.scale(m_scale);
	m_transform.rotate(m_rotation);
	m_transform.move(m_position);

	m_transform.project(0, 0, 1);

	for (auto& pt : m_points)
	{
		auto rs = pt.src*m_transform;
		pt.dst.setX(rs.at(0,0));
		pt.dst.setY(rs.at(0,1));
	}
}

void X3DObject::init(X3DView* view, X3DObject* parent)
{
	m_view = view;
	m_parent = parent;
	if (m_view)
	{
		m_view->addObject(this);
	}
	if (m_parent)
	{
		m_parent->addChild(this);
	}

	m_position.set(0, 0, 0);
	m_rotation.set(0, 0, 0);
	m_scale.set(1, 1, 1);
}

int X3DObject::indexOfPoint(const XVector3& src)
{
	XVector4 src2(src, 1);

	int count = m_points.count();
	for (int i = 0; i < count; i++)
	{
		if (m_points.at(i).src == src2)
			return i;
	}
	
	Point point(src2);
	m_points.append(point);
	return count;
}
