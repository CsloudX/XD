#ifndef X_PAUSE_DIALOG_H
#define X_PAUSE_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QThread>
#include <QFileInfo>
#include <QDebug>

class XPauseDialog : public QDialog
{
	Q_OBJECT
public:
	XPauseDialog(QWidget* parent=0)
		:QDialog(parent)
	{
		m_label = new QLabel(this);
		QPushButton* button = new QPushButton(tr("OK"), this);
		connect(button, SIGNAL(clicked()), this, SLOT(close()));
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->addWidget(m_label);
		layout->addWidget(button);
		setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
	}
	
	void pause(const QString& text)
	{
		static QPoint pos;

		move(pos);
		show();
		m_label->setText(text);
		resize(10, 10);
		m_wantPause = true;
		while (m_wantPause) {
			QCoreApplication::processEvents();
			QThread::msleep(10);
		}

		pos = this->pos();
		accept();
	}

protected:
	void closeEvent(QCloseEvent* event)
	{
		m_wantPause = false;
		QDialog::closeEvent(event);
	}
	
private:
	volatile bool m_wantPause;
	QLabel* m_label;
};

#define  X_PAUSE_DLG(text) { \
	XPauseDialog dlg;	\
	dlg.pause(QString("%1(%2)\n%3") \
		.arg(QFileInfo(__FILE__).fileName()).arg(__LINE__).arg(text));\
	qDebug()<<"X_PAUSE_DLG: "<<text; \
}

#endif //X_PAUSE_DIALOG_H
