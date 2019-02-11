#ifndef _XLABEL_HELPER_H
#define _XLABEL_HELPER_H

#include <QLabel>
#include <QHash>
#include <QTimer>
#include <QLineEdit>
#include <QEvent>
#include <QDebug>

class XLabelHelper : public QObject{
	Q_OBJECT

signals:
	void textChanged(const QString& text);

public:
	~XLabelHelper(){instances()->remove(_obj);}
	static XLabelHelper* getInstance(QLabel* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XLabelHelper(obj);
		}
		return (*instances())[obj];
	}
public slots:
	void flash(const QString& text = QString(), int timeout = 1000){
		if (!text.isEmpty())
			_obj->setText(text);
		_flashCount = timeout / _flashTimer.interval();
		_flashTimer.start();
	}
	void setText(const QString& text){
		if (_srcText != text) {
			_obj->setText(text);
			_srcText = text;
			emit textChanged(text);
		}
	}
	void setDoubleClickEdit(bool yes) { m_doubleClickEdit = yes; }

private:
    XLabelHelper(QLabel* obj) :QObject(obj), _obj(obj),m_doubleClickEdit(false){ 
        Q_ASSERT(_obj);
		_srcText = obj->text();
		_flashTimer.setInterval(100);
		connect(&_flashTimer, SIGNAL(timeout()), this, SLOT(innerFlash()));
		_obj->installEventFilter(this);
    }
	static QHash<QLabel*, XLabelHelper*>* instances(){
		static QHash<QLabel*, XLabelHelper*> s_instances;
		return &s_instances;
	}
private slots:
	void innerFlash(){
		static bool b = false;
		--_flashCount; 
		if (_flashCount < 0){
			_flashTimer.stop();
			_obj->setStyleSheet("");
			_obj->setText(_srcText);
		}

		b = !b;
		_obj->setStyleSheet(b ? "color:red;" : "color:blue;");
	}

protected:
	bool eventFilter(QObject *watched, QEvent *event)
	{
		if (watched == _obj && event->type() == QEvent::MouseButtonDblClick && m_doubleClickEdit) {
			QLineEdit* edit = new QLineEdit(_obj);
			edit->resize(_obj->size());
			edit->setText(_obj->text());
			edit->setAlignment(_obj->alignment());
			edit->show();
			edit->setFocus();
			connect(edit, &QLineEdit::editingFinished, [=]() {
				setText(edit->text());
				edit->deleteLater();
			});
		}
		return QObject::eventFilter(watched, event);
	}
private:
	QLabel*	_obj;
	QString _srcText;
	QTimer	_flashTimer;
	int		_flashCount;
	bool	m_doubleClickEdit;
};

#define XLABEL_HELPER(obj)  XLabelHelper::getInstance(obj)

#endif // _XLABEL_HELPER
