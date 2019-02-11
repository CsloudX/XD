#ifndef _X_SIGNAL_BUFFER_H
#define _X_SIGNAL_BUFFER_H

#include <QObject>
#include <QTime>
#include <QTimer>

class XSignalBuffer : public QObject
{
	Q_OBJECT
signals:
	void triggered();

public:
	XSignalBuffer(QObject* parent = nullptr) : QObject(parent)
	{ setup(1000); }
	XSignalBuffer(int interval, QObject* parent = nullptr) : QObject(parent)
	{ setup(interval); }
	XSignalBuffer(int interval, QObject *receiver, const char *method):QObject(receiver)
	{
		setup(interval);
		connect(this, SIGNAL(triggered()), receiver, method);
	}

	void setInterval(int interval){ m_interval = interval; m_timer.setInterval(m_interval); }
	int interval()const{ return m_interval; }

	static XSignalBuffer* create(QObject *sender, const char *signal, QObject *receiver, const char *method, int interval=1000)
	{
		XSignalBuffer* signalBuffer=new XSignalBuffer(interval, receiver,method);
		QObject::connect(sender, signal, signalBuffer, SLOT(trigger()));
		return signalBuffer;
	}

public slots:
	void trigger()
	{
		if (!m_timer.isActive())
		{
			m_timer.start();
		}
	}
	void triggerNow()
	{
		m_timer.stop();
		emit triggered();
	}

private:
	void setup(int interval)
	{
		setInterval(interval);
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(triggerNow()));
	}
	
private:
	int		m_interval;
	QTimer	m_timer;
};

#endif //_X_SIGNAL_BUFFER_H
