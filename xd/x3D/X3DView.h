#pragma once

#include <QWidget>
#include <QTransform>
#include "X3DObject.h"

class X3DView : public QWidget
{
	Q_OBJECT

public:
	X3DView(QWidget *parent);
	~X3DView();

	void addObject(X3DObject* object);


protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void childEvent(QChildEvent *event);

private:
	void updateTransform();


private:
	QTransform m_transform;
	QList<X3DObject*> m_objects;
};
