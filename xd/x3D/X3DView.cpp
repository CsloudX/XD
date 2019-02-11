#include "X3DView.h"
#include <QPainter>
#include <QPolygonF>
#include <QTransform>
#include <QPen>
#include <QChildEvent>

X3DView::X3DView(QWidget *parent)
	: QWidget(parent)
{
}

X3DView::~X3DView()
{
}


void X3DView::addObject(X3DObject* object)
{
	object->QObject::setParent(this);
	m_objects.append(object);
	update();
}

void X3DView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setTransform(m_transform);

	QPen pen = painter.pen();
	pen.setWidthF(0);
	painter.setPen(pen);


	for(auto object:m_objects)
	{
		painter.save();
		object->updateEvent(&painter);
		painter.restore();
	}
}

void X3DView::resizeEvent(QResizeEvent *event)
{
	updateTransform();
}

void X3DView::childEvent(QChildEvent *event)
{
	if (event->removed())
		m_objects.removeOne(qobject_cast<X3DObject*>(event->child()));
}

void X3DView::updateTransform()
{
	m_transform.reset();
	m_transform.translate(width() / 2, height() / 2);
	m_transform.scale(50, -50);
}
