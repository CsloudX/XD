#ifndef X_SPINBOX_HELPER_H
#define X_SPINBOX_HELPER_H


#include <QObject>
#include <QMap>
#include <QEvent>
#include <QMouseEvent>
#include <QAbstractSpinBox>

class XSpinBoxHelper : public QObject
{
	Q_OBJECT

public:
	~XSpinBoxHelper(){instances()->remove(m_obj);}
	static XSpinBoxHelper* getInstance(QAbstractSpinBox* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XSpinBoxHelper(obj);
		}
		return (*instances())[obj];
	}

	void setStepWidget(QWidget* widget, Qt::Orientation orientation=Qt::Horizontal)
	{
		if (m_stepWidget){
			m_stepWidget->removeEventFilter(this);
			m_stepWidget->setCursor(m_stepWidgetSrcCursor);
		}

		m_stepWidget = widget;
		if (!m_stepWidget)
			return;

		m_stepOrientation = orientation;
		m_stepWidgetSrcCursor = m_stepWidget->cursor();
		m_stepWidget->setCursor(m_stepOrientation == Qt::Horizontal ? Qt::SizeHorCursor : Qt::SizeVerCursor);
		m_stepWidget->installEventFilter(this);
	}

protected:
	bool eventFilter(QObject *obj, QEvent *event)
	{
		if (obj == m_stepWidget)
		{
			switch (event->type())
			{
			case QEvent::MouseButtonPress:
				m_stepWidgetPressed = true;
				m_stepWidgetPressPos = static_cast<QMouseEvent*>(event)->pos();
				break;
			case QEvent::MouseButtonRelease:
				m_stepWidgetPressed = false;
				break;
			case QEvent::MouseMove:
				if (m_stepWidgetPressed)
				{
					QPoint pos = static_cast<QMouseEvent*>(event)->pos();
					int offset = 0;
					switch (m_stepOrientation)
					{
					case Qt::Horizontal:
						offset = pos.x() - m_stepWidgetPressPos.x();
						break;
					case Qt::Vertical:
						offset =   m_stepWidgetPressPos.y()- pos.y();
						break;
					}
					if (offset > 0)	m_obj->stepUp();
					else if (offset < 0) m_obj->stepDown();
					m_stepWidgetPressPos = pos;
				}
				break;
			default:
				break;
			}
		}
		return QObject::eventFilter(obj, event);
	}

private:
    XSpinBoxHelper(QAbstractSpinBox* obj) 
		:QObject(obj), m_obj(obj),m_stepWidget(Q_NULLPTR),m_stepWidgetPressed(false){
        Q_ASSERT(m_obj);
        // code here
    }
	static QMap<QAbstractSpinBox*, XSpinBoxHelper*>* instances(){
		static QMap<QAbstractSpinBox*, XSpinBoxHelper*> s_instances;
		return &s_instances;
	}
private:
	QAbstractSpinBox*	m_obj;
	QWidget*		m_stepWidget;
	QCursor			m_stepWidgetSrcCursor;
	Qt::Orientation m_stepOrientation;
	bool			m_stepWidgetPressed;
	QPoint			m_stepWidgetPressPos;
};

#define X_SPINBOX_HELPER(obj) XSpinBoxHelper::getInstance(obj)


#endif // X_SPINBOX_HELPER_H
