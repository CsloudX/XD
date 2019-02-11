#ifndef _X_CONFIG_H
#define _X_CONFIG_H

#include <QSettings>
#include <QDialog>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QHash>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>
#include <QPainter>
#include <XTimeRecorder.hpp>

/************ Example ***********
#define STU_CONFIG_LIST(_) \
	_(ID			,0) \
	_(Name			,"") 
X_DEF_CONFIG_EX(Stu, STU_CONFIG_LIST)
*********************************/

#define _X_DEF_CONFIG_SET(key,def) void set##key(const QVariant& value){XConfig::setValue(#key,value);}
#define _X_DEF_CONFIG_GET(key,def) QVariant key()const{return XConfig::value(#key,def);}
#define _X_DEF_CONFIG_NAME(key,def) static QString key##Name(){ return #key;}
#define  X_DEF_CONFIG(Name,FileName,List) \
	class Name##Config : public XConfig{ \
    public:Name##Config(QObject* parent=nullptr):XConfig(#FileName".ini",parent){} \
    List(_X_DEF_CONFIG_NAME) \
    List(_X_DEF_CONFIG_GET) \
    List(_X_DEF_CONFIG_SET) };

#define X_DEF_CONFIG_EX(Name,List) X_DEF_CONFIG(Name,Name,List)

class XConfig : public QObject
{
	Q_OBJECT
signals:
	void valueChanged(const QString& key, const QVariant& value);
	
public:
	XConfig(const QString& fileName, QObject* parent = nullptr) 
		:QObject(parent)
		, m_settings(new QSettings(fileName,QSettings::IniFormat, this))
	{

	}
	virtual ~XConfig(){}

	QVariant value(const QString& key, const QVariant &defaultValue = QVariant())const
	{
		return m_settings->value(key, defaultValue);
	}

public slots:
	void setValue(const QString& key, const QVariant& value)
	{
		if (m_settings->value(key) != value)
		{
			m_settings->setValue(key, value);
			emit valueChanged(key, value);
		}
	}
	
private:
	bool		m_needSave;
	QSettings*	m_settings;
};

class XConfigDialogItem : public QWidget
{
	Q_OBJECT
signals:
	void valueChanged(const QString& key, const QVariant& value);

public:
	XConfigDialogItem(QWidget* parent = nullptr) :QWidget(parent){}
	virtual ~XConfigDialogItem(){}
};

class XConfigDialog : public QDialog{

	Q_OBJECT

signals :
	void valueChanged(const QString& key, const QVariant& value);
	void okButtonClicked();
	void cancelButtonClicked();
	void applyButtonClicked();
	void configWantOk();
	void configWantCancel();

public:
	XConfigDialog(QWidget* parent = nullptr)
		:QDialog(parent){
		setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
		setWindowTitle(tr("Config"));

		m_lineEditFilter = new QLineEdit(this);
		m_lineEditFilter->setPlaceholderText(tr("Filter"));

		m_listCategory = new QListWidget(this);

		m_labelHeader = new QLabel(this);
		QFont font = m_labelHeader->font();
		font.setBold(true);
		m_labelHeader->setFont(font);
		m_labelHeader->setAlignment(Qt::AlignCenter);

		m_layoutWidget = new QStackedWidget(this);

		m_buttonBox = new QDialogButtonBox(
			QDialogButtonBox::Ok |
			QDialogButtonBox::Cancel |
			QDialogButtonBox::Apply,
			this);
		connect(m_buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(onDialogButtonBoxClicked(QAbstractButton *)));

		m_splitter = new QSplitter(this);
		m_splitter->setOrientation(Qt::Horizontal);
		m_splitter->setChildrenCollapsible(false);
		m_leftWidget = new QWidget(m_splitter);
		m_leftWidget->setLayout(new QVBoxLayout());
		m_leftWidget->layout()->addWidget(m_lineEditFilter);
		m_leftWidget->layout()->addWidget(m_listCategory);
		m_leftWidget->layout()->setMargin(0);
		QWidget* wRight = new QWidget(m_splitter);
		wRight->setLayout(new QVBoxLayout(wRight));
		wRight->layout()->addWidget(m_labelHeader);
		wRight->layout()->addWidget(m_layoutWidget);
		wRight->layout()->setMargin(0);

		this->setLayout(new QVBoxLayout(this));
		this->layout()->addWidget(m_splitter);
		this->layout()->addWidget(m_buttonBox);


		//_listCategory.setIconSize(QSize(24, 24));
		connect(m_listCategory, SIGNAL(currentRowChanged(int)), m_layoutWidget, SLOT(setCurrentIndex(int)));
		connect(m_listCategory, SIGNAL(currentTextChanged(const QString&)), m_labelHeader, SLOT(setText(const QString&)));

		connect(m_lineEditFilter, SIGNAL(textChanged(const QString&)), this, SLOT(filter(const QString&)));
	}
	int addItem(QWidget* item,const QString& text, const QIcon& icon = QIcon(), bool isSignal=true)
	{
		new QListWidgetItem(icon, text, m_listCategory);
		int index = m_layoutWidget->addWidget(item);

		if (isSignal)
		{
			connect(item, SIGNAL(valueChanged(const QString&, const QVariant&)),
				this, SLOT(setValue(const QString&, const QVariant&)));
		}
		return index;
	}
	QWidget* leftWiget()const{ return m_leftWidget; }
	QSplitter* splitter()const{ return m_splitter; }

public slots:
	void setValue(const QString& key, const QVariant& val){ m_values[key] = val; }
	int exec(int idx = 0){
		m_listCategory->setCurrentRow(idx);
		return QDialog::exec();
	}
	void show(int idx = 0){
		m_listCategory->setCurrentRow(idx);
		QDialog::show();
	}

private:
	const QStringList& widgetStrings(QWidget* w){
		Q_ASSERT(w);
		if (!m_widgetStrings.contains(w)){
			m_widgetStrings[w] = widgetStringList(w);
		}
		return m_widgetStrings[w];
	}

	QStringList widgetStringList(QWidget* w){
		Q_ASSERT(w);
		QStringList stringList;
		foreach(const QLabel* label, w->findChildren<QLabel*>())
			stringList << label->text();
		foreach(const QPushButton* pushButton, w->findChildren<QPushButton*>())
			stringList << pushButton->text();
		foreach(const QCheckBox* checkBox, w->findChildren<QCheckBox*>())
			stringList << checkBox->text();
		foreach(const QGroupBox* groupBox, w->findChildren<QGroupBox*>())
			stringList << groupBox->title();

		QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
		if (tabWidget){
			int tabCount = tabWidget->count();
			for (int i = 0; i < tabCount; ++i) {
				stringList << tabWidget->tabText(i);
			}
		}


		// clean up accelerators
		QMutableStringListIterator it(stringList);
		while (it.hasNext())
			it.next().remove(QLatin1Char('&'));

		return stringList;
	}

	bool filterCategory(int idx, const QString& str)
	{
		if (m_listCategory->item(idx)->text().contains(str, Qt::CaseInsensitive))
			return true;

		QWidget* w = m_layoutWidget->widget(idx);
		foreach(const QString& var, widgetStrings(w)){
			if (var.contains(str, Qt::CaseInsensitive))
				return true;
		}
		return false;
	}
	bool filterTabWidget(QTabWidget* tabWidget, int idx, const QString& str)
	{
		if (tabWidget->tabText(idx).contains(str, Qt::CaseInsensitive))
			return true;
		QWidget* w = tabWidget->widget(idx);
		foreach(const QString& var, widgetStrings(w)){
			if (var.contains(str, Qt::CaseInsensitive))
				return true;
		}
		return false;
	}
	void setWidgetHorizontalPolicy(QWidget* w, QSizePolicy::Policy horizontal){
		QSizePolicy policy = w->sizePolicy();
		policy.setHorizontalPolicy(horizontal);
		w->setSizePolicy(policy);
	}

	void apply(){
		QHash<QString, QVariant>::const_iterator itr = m_values.constBegin();
		while (itr != m_values.constEnd()){
			emit valueChanged(itr.key(), itr.value());
			++itr;
		}
		m_values.clear();
	}

private slots:
	void filter(const QString& str)
	{
		int count = m_listCategory->count();
		for (int i = 0; i < count; ++i){
			bool enable = filterCategory(i, str);
			m_listCategory->setRowHidden(i, !enable);
			if (enable){
				QTabWidget* tabWidget = qobject_cast<QTabWidget*>(m_layoutWidget->widget(i));
				if (tabWidget){
					int tabCount = tabWidget->count();
					for (int tabIdx = 0; tabIdx < tabCount; ++tabIdx)
						tabWidget->setTabEnabled(tabIdx, filterTabWidget(tabWidget, tabIdx, str));
				}
			}
		}

		m_labelHeader->setVisible(true);
		m_layoutWidget->setVisible(true);
		for (int i = 0; i < count; ++i){
			if (!m_listCategory->isRowHidden(i)){
				m_listCategory->setCurrentRow(i);
				return;
			}
		}
		m_labelHeader->setVisible(false);
		m_layoutWidget->setVisible(false);
	}
	void onDialogButtonBoxClicked(QAbstractButton *button){
		auto standardButton = m_buttonBox->standardButton(button);
		switch (standardButton)
		{
		case QDialogButtonBox::Ok:
			apply();
			accept();
			emit configWantOk();
			emit okButtonClicked();
			break;
		case QDialogButtonBox::Cancel:
			m_values.clear();
			reject();
			emit cancelButtonClicked();
			emit configWantCancel();
			break;
		case QDialogButtonBox::Apply:
			apply();
			emit configWantOk();
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

protected:
	QLineEdit*			m_lineEditFilter;
	QListWidget*		m_listCategory;
	QWidget*			m_leftWidget;
	QLabel*				m_labelHeader;
	QStackedWidget*     m_layoutWidget;
	QSplitter*			m_splitter;
	QDialogButtonBox*   m_buttonBox;
	QHash<QWidget*, QStringList>	m_widgetStrings;
	QHash<QString, QVariant>        m_values;
};

#endif //_X_CONFIG_H
