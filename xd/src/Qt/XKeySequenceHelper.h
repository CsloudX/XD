#ifndef _X_KEY_SEQUENCE_HELPER_H
#define _X_KEY_SEQUENCE_HELPER_H

#include <QKeySequence>

#include <gl/GL.h>


class XKeySequenceHelper{

public:
	XKeySequenceHelper(const QKeySequence& key):m_key(key){}
	~XKeySequenceHelper(){}

	Qt::KeyboardModifiers 

private:
	QKeySequence	m_key;
};


#endif // _X_KEY_SEQUENCE_HELPER_H
