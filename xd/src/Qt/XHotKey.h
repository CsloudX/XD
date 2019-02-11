#ifndef _X_HOT_KEY_H_
#define _X_HOT_KEY_H_

#include <QWidget>
#include <QKeySequence>
#include <QMap>
#include <qt_windows.h>

class XHotKey : public QWidget
{
	Q_OBJECT
	
public:
	XHotKey(QWidget* parent = 0)
		:QWidget(parent)
	{
		setFixedSize(0,0);
		setVisible(false);
	}

	bool registerHotKey(int id, const QKeySequence& key)
	{
		Q_ASSERT(m_registeredKeys.find(id) == m_registeredKeys.end());

		return true;
	}

private:
	QMap<int, QKeySequence> m_registeredKeys;
};

#endif //_X_HOT_KEY_H_
