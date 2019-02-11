#ifndef _X_CHAR_WIDGET_H
#define _X_CHAR_WIDGET_H

#include <QWidget>
#include <QChar>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QPaintEvent>

class XCharWidget: public QWidget
{
	Q_OBJECT

signals:
	void charChanged(const QChar& ch);

public:
	XCharWidget(const QChar& ch, QWidget* parent = nullptr)
		:QWidget(parent), m_ch(ch){}
	XCharWidget(QWidget* parent = nullptr)
		:QWidget(parent), m_ch(0){}

public slots:
	void setChar(const QChar& ch)
	{
		if (m_ch != ch)
		{
			m_ch = ch;
			emit charChanged(m_ch);
			update();
		}
	}

protected:
	void paintEvent(QPaintEvent* event)
	{

		QPainter painter(this);
		int fontSize = qMin(width(), height()) / 1.8497;
		QFont font = this->font();
		font.setPixelSize(fontSize);
		painter.setFont(font);

		QStyleOptionButton opt;
		opt.initFrom(this);
		opt.text = QString(m_ch);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
		//style()->drawControl(QStyle::CE_PushButtonLabel, &opt, &painter, this);
		style()->drawItemText(&painter, rect(), Qt::AlignCenter, opt.palette, isEnabled()
			, QString(m_ch));
		//painter.drawText(rect(), QString(m_ch), QTextOption(Qt::AlignCenter));
	}
	//bool event(QEvent* ev)
	//{
	//	if (ev->type() == QEvent::Enter
	//		|| ev->type() == QEvent::Leave)
	//	{
	//		style()->unpolish(this);
	//		style()->polish(this);
	//		qDebug() << "ssss";
	//	}
	//	return QWidget::event(ev);
	//}

private:
	QChar	m_ch;
 };

#endif //_X_CHAR_WIDGET_H
