#ifndef _XSHORTCUTWIDGETHELPER_H_
#define _XSHORTCUTWIDGETHELPER_H_

#include <QObject>
#include <QHash>
#include <QLineEdit>

class XShortcutWidgetHelper : public QObject
{
	Q_OBJECT

public:
	~XShortcutWidgetHelper(){instances()->remove(m_obj);}
	static XShortcutWidgetHelper* getInstance(QLineEdit* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XShortcutWidgetHelper(obj);
		}
		return (*instances())[obj];
	}
private:
    XShortcutWidgetHelper(QLineEdit* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
        // code here
    }
	static QHash<QLineEdit*, XShortcutWidgetHelper*>* instances(){
		static QHash<QLineEdit*, XShortcutWidgetHelper*> s_instances;
		return &s_instances;
	}
private:
	QLineEdit*	m_obj;
};

#define XSHORTCUTWIDGETHELPER(obj) XShortcutWidgetHelper::getInstance(obj)

#endif // _XSHORTCUTWIDGETHELPER_H_
