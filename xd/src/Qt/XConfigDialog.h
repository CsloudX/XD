#ifndef _X_CONFIG_DIALOG_H
#define _X_CONFIG_DIALOG_H

#include <QDialog>
#include <QStyleOption>
#include <QPainter>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QSplitter>
#include <QDialogButtonBox>

class XConfigDialog : public QDialog
{
	Q_OBJECT

signals:
	void wantApply();
	void wantCancel();

public:
	XConfigDialog(QWidget* parent=nullptr):QDialog(parent)
	{
		setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
		setWindowTitle(tr("Config"));

		m_filterEdit = new QLineEdit(this);
		m_filterEdit->setPlaceholderText(tr("Filter"));
		m_listWidget = new QListWidget(this);
		m_titleLabel = new QLabel(this);
		m_titleLabel->setAlignment(Qt::AlignCenter);
		m_stackedWidget = new QStackedWidget(this);
		m_buttonBox = new QDialogButtonBox(
			QDialogButtonBox::Ok |
			QDialogButtonBox::Cancel |
			QDialogButtonBox::Apply, this);
		connect(m_buttonBox, SIGNAL(clicked(QAbstractButton *)), 
			this, SLOT(onDialogButtonBoxClicked(QAbstractButton *)));
	}

private slots:
	void onDialogButtonBoxClicked(QAbstractButton *button) {
		auto standardButton = m_buttonBox->standardButton(button);
		switch (standardButton)
		{
		case QDialogButtonBox::Ok:
			apply();
			accept();
			emit okButtonClicked();
			break;
		case QDialogButtonBox::Cancel:
			m_values.clear();
			reject();
			emit cancelButtonClicked();
			break;
		case QDialogButtonBox::Apply:
			apply();
			emit applyButtonClicked();
			break;
		default:
			break;
		}
	}

protected:
	void paintEvent(QPaintEvent *)
	{
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

private:
	QLineEdit*		m_filterEdit;
	QListWidget*	m_listWidget;
	QLabel*			m_titleLabel;
	QStackedWidget* m_stackedWidget;
	QDialogButtonBox* m_buttonBox;
};

#endif //_X_CONFIG_DIALOG_H
