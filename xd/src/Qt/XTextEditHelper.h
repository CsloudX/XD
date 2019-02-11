#ifndef _X_TEXT_EDIT_HELPER_H
#define _X_TEXT_EDIT_HELPER_H

#include <QTextEdit>
#include <QHash>
#include <QabstractButton>
#include <QFontComboBox>
#include <QDebug>
#include "XAlignmentEditor.h"

class _XQFontSizeComboHelper : public QObject
{
    Q_OBJECT
signals:
    void fontSizeChanged(int size);
public:
    _XQFontSizeComboHelper(QComboBox* combo)
        :QObject(combo),m_combo(combo)
    {
        Q_ASSERT(m_combo);
        m_combo->setEditable(true);
        const QList<int> standardSizes = QFontDatabase::standardSizes();
        foreach (int size, standardSizes)
            m_combo->addItem(QString::number(size));
        connect(m_combo, QOverload<const QString &>::of(&QComboBox::activated),
                [=](const QString& s){
            bool ok=false;
            int size = s.toInt(&ok);
            if(ok) emit fontSizeChanged(size);
        });
    }

public slots:
    void setFontSize(int size){m_combo->setCurrentText(QString::number(size));}


private:
    QComboBox* m_combo;
};

class XTextEditHelper : public QObject{
	Q_OBJECT

signals:
	void fontChanged(const QFont& font);
	void fontFamilyChanged(const QString& family);
	void fontSizeChanged(int sz);
	void boldChanged(bool b);
	void italicChanged(bool b);
	void underlineChanged(bool b);
	void colorChanged(const QColor& c);
	void backgroundColorChanged(const QColor& c);
	void strikeOutChanged(bool b);
	void superScriptChanged(bool b);
	void subScriptChanged(bool b);
    void alignmentChanged(Qt::Alignment align);

public:
	~XTextEditHelper(){instances()->remove(m_obj);}
	static XTextEditHelper* getInstance(QTextEdit* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XTextEditHelper(obj);
		}
		return (*instances())[obj];
	}
	void setSuperScriptButton(QAbstractButton* btn)
	{
		Q_ASSERT(btn);
		btn->setCheckable(true);
		connect(btn, SIGNAL(toggled(bool)), this, SLOT(setSuperScript(bool)));
		connect(this, SIGNAL(superScriptChanged(bool)), btn, SLOT(setChecked(bool)));
	}
	void setSubScriptButton(QAbstractButton* btn)
	{
		Q_ASSERT(btn);
		btn->setCheckable(true);
		connect(btn, SIGNAL(toggled(bool)), this, SLOT(setSubScript(bool)));
		connect(this, SIGNAL(subScriptChanged(bool)), btn, SLOT(setChecked(bool)));
	}
    void setupFontFamilyComboBox(QFontComboBox* combo)
    {
        connect(combo,SIGNAL(activated(QString)),this,SLOT(setFontFamily(QString)));
        connect(this,SIGNAL(fontFamilyChanged(QString)),combo,SLOT(setCurrentText(QString)));
    }
    void setupFontSizeComboBox(QComboBox* combo)
    {
        auto w =new _XQFontSizeComboHelper(combo);
        connect(w,SIGNAL(fontSizeChanged(int)),this,SLOT(setFontSize(int)));
        connect(this,SIGNAL(fontSizeChanged(int)),w,SLOT(setFontSize(int)));
    }
    void setupUnderlineButton(QAbstractButton* btn)
    {
        btn->setCheckable(true);
        connect(btn,SIGNAL(clicked(bool)),this,SLOT(setUnderline(bool)));
        connect(this,SIGNAL(underlineChanged(bool)),btn,SLOT(setChecked(bool)));
    }
    void setupSuperScriptButton(QAbstractButton* btn)
    {
        btn->setCheckable(true);
        connect(btn,SIGNAL(clicked(bool)),this,SLOT(setSuperScript(bool)));
        connect(this,SIGNAL(superScriptChanged(bool)),btn,SLOT(setChecked(bool)));
    }
    void setupSubScriptButton(QAbstractButton* btn)
    {
        btn->setCheckable(true);
        connect(btn,SIGNAL(clicked(bool)),this,SLOT(setSubScript(bool)));
        connect(this,SIGNAL(subScriptChanged(bool)),btn,SLOT(setChecked(bool)));
    }
    void setupHAlignButtons(QAbstractButton* left,  QAbstractButton* hcenter,
                            QAbstractButton* right, QAbstractButton* justify = nullptr)
    {
        auto w = new XAlignmentEditor(this);
        w->setHorizontalWidgets(left,hcenter,right,justify);
        connect(w,SIGNAL(alignmentChanged(Qt::Alignment)),this,SLOT(setAlignment(Qt::Alignment)));
        connect(this,SIGNAL(alignmentChanged(Qt::Alignment)),w,SLOT(setAlignment(Qt::Alignment)));
    }

public slots:
	void setFont(const QFont& font)
	{
		QTextCharFormat fmt;
		fmt.setFont(font);
		mergeCharFormat(fmt);
	}
	void setFontFamily(const QString& family)
	{
		QTextCharFormat fmt;
		fmt.setFontFamily(family);
		mergeCharFormat(fmt);
	}
	void setFontSize(int sz)
	{
		QTextCharFormat fmt;
		fmt.setFontPointSize(sz);
		mergeCharFormat(fmt);
	}
	void setBold(bool b)
	{
		QTextCharFormat fmt;
		fmt.setFontWeight(b ? QFont::Bold : QFont::Normal);
		mergeCharFormat(fmt);
	}
	void setItalic(bool b)
	{
		QTextCharFormat fmt;
		fmt.setFontItalic(b);
		mergeCharFormat(fmt);
	}
	void setUnderline(bool b)
	{
		QTextCharFormat fmt;
		fmt.setFontUnderline(b);
		mergeCharFormat(fmt);
	}
    void setAlignment(Qt::Alignment align)
    {
        qDebug()<<align<<"---";
        m_obj->setAlignment(align);
    }
	void setColor(const QColor& c)
	{
		QTextCharFormat fmt;
		if (c.isValid())
			fmt.setForeground(c);
		else
			fmt.clearForeground();
		mergeCharFormat(fmt);
	}
	void setBackgroundColor(const QColor& c)
	{
		QTextCharFormat fmt;
		if (c.isValid())
			fmt.setBackground(c);
		else
			fmt.setBackground(QColor(0,0,0,0));
		mergeCharFormat(fmt);
	}
	void setStrikeOut(bool b)
	{
		QTextCharFormat fmt;
		fmt.setFontStrikeOut(b);
		mergeCharFormat(fmt);
	}
	void setSuperScript(bool b)
	{
		QTextCharFormat fmt;
		fmt.setVerticalAlignment(b?QTextCharFormat::AlignSuperScript:QTextCharFormat::AlignNormal);
		mergeCharFormat(fmt);
	}
	void setSubScript(bool b)
	{
		QTextCharFormat fmt;
		fmt.setVerticalAlignment(b ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal);
		mergeCharFormat(fmt);
	}
	void setLink(const QString& href=0)
	{
		auto cursor = m_obj->textCursor();
		auto text = cursor.selectedText();
		if (text.isEmpty())
			return;

		insertLink(text, (href == 0)?text: href);
	}
	void insertLink(const QString& text,const QString& href)
	{
		Q_ASSERT(!text.isEmpty());
		Q_ASSERT(!href.isEmpty());
		m_obj->textCursor().insertHtml(QString("<a href=\"%1\">%2</a>").arg(href).arg(text));
	}
	void clearFormat()
	{
		auto cursor = m_obj->textCursor();
		auto text = cursor.selectedText();
		cursor.setCharFormat(QTextCharFormat());
		m_obj->setCurrentCharFormat(QTextCharFormat());
		cursor.insertText(text);
	}

private slots:
	void onCurrentCharFormatChanged(const QTextCharFormat& fmt)
	{
		QFont font = fmt.font();
		emit fontChanged(font);
		emit fontFamilyChanged(font.family());
		emit fontSizeChanged(font.pointSize());
		emit boldChanged(font.bold());
		emit italicChanged(font.italic());
		emit underlineChanged(font.underline());
		emit strikeOutChanged(font.strikeOut());
		emit superScriptChanged(fmt.verticalAlignment() == QTextCharFormat::AlignSuperScript);
		emit subScriptChanged(fmt.verticalAlignment() == QTextCharFormat::AlignSubScript);
		emit colorChanged(fmt.foreground().color());

		auto brush = fmt.background();
		emit backgroundColorChanged(brush.style() == Qt::NoBrush ? QColor() : brush.color());
	}
    void onCursorPositionChanged()
    {
        emit alignmentChanged(m_obj->alignment());
    }

private:
    XTextEditHelper(QTextEdit* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
		connect(m_obj, SIGNAL(currentCharFormatChanged(const QTextCharFormat&)),
			this, SLOT(onCurrentCharFormatChanged(const QTextCharFormat& )));
        connect(m_obj,SIGNAL(cursorPositionChanged()),this,SLOT(onCursorPositionChanged()));
    }
	void mergeCharFormat(const QTextCharFormat& fmt)
	{
		m_obj->mergeCurrentCharFormat(fmt);
		m_obj->textCursor().mergeCharFormat(fmt);
	}

	static QHash<QTextEdit*, XTextEditHelper*>* instances(){
		static QHash<QTextEdit*, XTextEditHelper*> s_instances;
		return &s_instances;
	}
private:
	QTextEdit*	m_obj;
};

#define X_TEXT_EDIT_HELPER(obj)  XTextEditHelper::getInstance(obj)

#endif // _X_TEXT_EDIT_HELPER_H
