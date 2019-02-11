#ifndef _XLINE_EDIT_HELPER_H
#define _XLINE_EDIT_HELPER_H

#include <QLineEdit>
#include <QHash>

class XLineEditHelper :public QObject
{
	Q_OBJECT

signals:
	void editingFinished(const QString& text);

public:
	~XLineEditHelper(){instances()->remove(m_obj);}
	static XLineEditHelper* getInstance(QLineEdit* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XLineEditHelper(obj);
		}
		return (*instances())[obj];
	}
private:
    XLineEditHelper(QLineEdit* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
		connect(m_obj, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
    }
	static QHash<QLineEdit*, XLineEditHelper*>* instances(){
		static QHash<QLineEdit*, XLineEditHelper*> s_instances;
		return &s_instances;
	}

private slots:
	void onEditingFinished()
	{
		emit editingFinished(m_obj->text());
	}

private:
	QLineEdit*	m_obj;
};

#define XLINE_EDIT_HELPER(obj)  XLineEditHelper::getInstance(obj)

#endif // _XLINE_EDIT_HELPER
