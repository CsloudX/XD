#ifndef _X_ANIMATION_WIDGET_H
#define _X_ANIMATION_WIDGET_H

#include <QWidget>
#include <QList>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QDebug>

class XWidgetAnimation : public QParallelAnimationGroup
{
	Q_OBJECT

public:
	XWidgetAnimation(QObject* parent = nullptr) 
		:QParallelAnimationGroup(parent)
	{
		m_posAnimation = createPropertyAnimation("pos");
		m_posAnimation->setStartValue(QPoint(0, 0));
		m_posAnimation->setEndValue(QPoint(0, 0));

		m_sizeAnimation = createPropertyAnimation("size");
		m_sizeAnimation->setStartValue(QSize(0, 0));
		m_sizeAnimation->setEndValue(QSize(0, 0));
	}

	void setTargetObject(QWidget* target)
	{
		m_posAnimation->setTargetObject(target);
		m_sizeAnimation->setTargetObject(target);
	}
	QWidget* targetObject()const{ return qobject_cast<QWidget*>( m_posAnimation->targetObject()); }

	void setEndPos(const QPoint& pos){ m_posAnimation->setEndValue(pos); }
	QPoint endPos()const{ return m_posAnimation->endValue().toPoint(); }

	void setEndSize(const QSize& size){ m_sizeAnimation->setEndValue(size); }
	QSize endSize()const{ return m_sizeAnimation->endValue().toSize(); }

	QPoint startPos()const{ return m_posAnimation->startValue().toPoint(); }
	QSize startSize()const{ return m_sizeAnimation->startValue().toSize(); }

	void setDuration(int msecs)
	{
		m_posAnimation->setDuration(msecs);
		m_sizeAnimation->setDuration(msecs);
	}

private:
	QPropertyAnimation* createPropertyAnimation(const QByteArray &propertyName)
	{
		QPropertyAnimation* animation = new QPropertyAnimation(this);
		animation->setPropertyName(propertyName);
		addAnimation(animation);

		return animation;
	}


	void setStartPos(const QPoint& pos){ m_posAnimation->setStartValue(pos); }
	void setStartSize(const QSize& size){ m_sizeAnimation->setStartValue(size); }

private:
	QPropertyAnimation* m_posAnimation;
	QPropertyAnimation* m_sizeAnimation;

	friend class XAnimationWidget;
};

class XAnimationWidget : public QWidget
{
	Q_OBJECT

signals:
	void currentIndexChanged(int index);
	void currentWidgetChanged(QWidget* widget);

public:
	XAnimationWidget(QWidget* parent = nullptr)
		:QWidget(parent),m_preWidget(nullptr), m_currentWidget(nullptr)
	{
		m_toTargetAnimation = new XWidgetAnimation(this);

		m_fromTargetAnimation = new XWidgetAnimation(this);
		m_fromTargetAnimation->setDirection(QAbstractAnimation::Backward);

		connect(m_toTargetAnimation, SIGNAL(finished()), this, SLOT(onAnimationFinished()));
	}

	void addWidget(QWidget* widget)
	{
		Q_ASSERT(!m_widgets.contains(widget));

		m_widgets.append(widget);
		widget->setParent(this);
		if (m_currentWidget)
		{
			widget->setVisible(false);
		}
		else
		{
			widget->move(0, 0);
			widget->resize(this->size());
			widget->setVisible(true);
			m_currentWidget = widget;
		}
	}
	void removeWidget(QWidget* widget)
	{
		m_widgets.removeOne(widget);
		if (m_currentWidget == widget)
		{
			m_currentWidget = nullptr;
		}
	}
	void insertWidget(int index, QWidget* widget){ m_widgets.insert(index, widget); }

	int count()const{ return m_widgets.count(); }

	int currentIndex()const{ return indexOf(m_currentWidget); }
	QWidget* currentWidget()const{ return m_currentWidget; }

	int indexOf(QWidget* widget)const {  return m_widgets.indexOf(widget);  }
	QWidget* widget(int index)const{ return m_widgets.at(index); }

	XWidgetAnimation* fromTargetAnimation()const{ return m_fromTargetAnimation; }
	XWidgetAnimation* toTargetAnimation()const{ return m_toTargetAnimation; }

public slots:
	void setCurrentIndex(int index)
	{
		setCurrentWidget(widget(index));
	}
	void setCurrentWidget(QWidget* widget)
	{
		Q_ASSERT(m_widgets.contains(widget));

		if (m_currentWidget == widget)
		{
			onAnimationFinished();
			return;
		}

		m_preWidget = m_currentWidget;
		m_currentWidget = widget;

		preCurrentWidgetChangedEvent(m_preWidget, m_currentWidget);
		m_toTargetAnimation->stop();
		m_fromTargetAnimation->stop();
		m_toTargetAnimation->setTargetObject(m_preWidget);
		m_fromTargetAnimation->setTargetObject(m_currentWidget);
		if (m_preWidget)
			m_toTargetAnimation->start();
		if (m_currentWidget)
		{
			m_fromTargetAnimation->start();
			m_currentWidget->setVisible(true);
			m_currentWidget->raise();
		}

		emit currentIndexChanged(indexOf(m_currentWidget));
		emit currentWidgetChanged(m_currentWidget);
	}

private slots:
	void onAnimationFinished()
	{
		for (auto& w : m_widgets)
		{
			w->setVisible(w == m_currentWidget);
		}
	}

protected:
	virtual void resizeEvent(QResizeEvent *event)
	{
		m_toTargetAnimation->setStartSize(event->size());
		m_fromTargetAnimation->setStartSize(event->size());
		if (m_currentWidget)
		{
			m_currentWidget->resize(event->size());
		}
	}
	virtual void preCurrentWidgetChangedEvent(QWidget* oldWidget, QWidget* newWidget){}

private:
	QList<QWidget*> m_widgets;
	QWidget*		m_preWidget;
	QWidget*		m_currentWidget;

	XWidgetAnimation*	m_toTargetAnimation;
	XWidgetAnimation*	m_fromTargetAnimation;
};

#endif // _X_ANIMATION_WIDGET_H
