#ifndef _XPUSH_BUTTON_HELPER_H
#define _XPUSH_BUTTON_HELPER_H

#include <QPushButton>
#include <QHash>
#include <QTimer>
#include <QColorDialog>
#include <QFontDialog>
#include <QDebug>

class XPushButtonHelper : public QObject
{
	Q_OBJECT

signals:
	void colorChanged(const QColor& color);
	void fontChanged(const QFont& font);

public:
	enum Mode{ NoMode,ColorMode,FontMode };
	~XPushButtonHelper()
	{
		instances()->remove(_obj);
	}
	static XPushButtonHelper* getInstance(QPushButton* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XPushButtonHelper(obj);
		}
		return (*instances())[obj];
	}

	void setMode(Mode mode){ _mode = mode; }
	Mode mode()const{ return _mode; }
	const QColor& color()const { return m_color; }
	QFont font()const { return m_font; }

public slots:
	void flash(const QString& text = QString(), int timeout = 1000){
		if (!text.isEmpty())
			_obj->setText(text);
		_flashCount = timeout / _flashTimer.interval();
		_flashTimer.start();
	}
	void setText(const QString& text){
		_obj->setText(text);
		_srcText = text;
	}
	void setColor(const QColor& color)
	{
		if (m_color != color)
		{
			m_color = color;

			auto palette = _obj->palette();
			palette.setColor(QPalette::Button, m_color);
			_obj->setPalette(palette);
			_obj->setFlat(true);
			_obj->setAutoFillBackground(true);
			qDebug() << "color chaged";
			emit colorChanged(m_color);
		}
	}
	void setFont(const QFont& font)
	{
		if (m_font != font)
		{
			m_font = font;
			_obj->setText(QString("%1,%2").arg(m_font.family()).arg(m_font.pointSize()));
			QFont font1 = _obj->font();
			font1.setFamily(m_font.family());
			_obj->setFont(font1);
			emit fontChanged(font);
		}
	}
private:
    XPushButtonHelper(QPushButton* obj) :QObject(obj), _obj(obj),_mode(NoMode){ 
        Q_ASSERT(_obj);
		_srcText = obj->text();
		_flashTimer.setInterval(100);
		connect(_obj, SIGNAL(clicked()), this, SLOT(onPushButtonClicked()));
		connect(&_flashTimer, SIGNAL(timeout()), this, SLOT(innerFlash()));
    }
	static QHash<QPushButton*, XPushButtonHelper*>* instances(){
		static QHash<QPushButton*, XPushButtonHelper*> s_instances;
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
			return;
		}

		b = !b;
		_obj->setStyleSheet(b ? "color:red;" : "color:blue;");
	}

	void onPushButtonClicked()
	{
		switch (_mode)
		{
		case XPushButtonHelper::NoMode:
			break;
		case XPushButtonHelper::ColorMode: {
				QColor color = QColorDialog::getColor(m_color,_obj,tr("Select Color"),
					QColorDialog::ShowAlphaChannel);
				if (color.isValid())
				{
					setColor(color);
				}
			}
			break;
		case XPushButtonHelper::FontMode: {
			bool ok = false;
			QFont font = QFontDialog::getFont(&ok, m_font);
			if (ok)
				setFont(font);
		}
			break;
		default:
			break;
		}
	}

private:
	QPushButton*	_obj;
	QString _srcText;
	QTimer	_flashTimer;
	int		_flashCount;
	Mode	_mode;
	QColor	m_color;
	QFont	m_font;
};

#define XPUSH_BUTTON_HELPER(obj)  XPushButtonHelper::getInstance(obj)

#endif // _XPUSH_BUTTON_HELPER
