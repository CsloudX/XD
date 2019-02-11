#ifndef _XWIDGET_HELPER_H
#define _XWIDGET_HELPER_H

#include <QWidget>
#include <QHash>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QGraphicsColorizeEffect>
#include <QPropertyAnimation>
#include <QMenu>
#include <QDebug>

class XWidgetHelper : public QObject
{
	Q_OBJECT

signals:
	void filesDropped(const QStringList& files);
	void fileDropped(const QString& file);

public:
	~XWidgetHelper(){instances()->remove(_obj);}
	static XWidgetHelper* getInstance(QWidget* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XWidgetHelper(obj);
		}
		return (*instances())[obj];
	}
	void setFlashColor(const QColor& startColor, const QColor& endColor, int period)
	{
		m_flashStartColor = startColor;
		m_flashEndColor = endColor;
		m_flashPeriod = period;
	}
    void setMenu(QMenu* menu){
        m_menu=menu;
        _obj->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(_obj,SIGNAL(customContextMenuRequested(QPoint)),
                this,SLOT(popupMenu()));
    }
public slots:
	void setEnableMouseMoveWidget(bool enable){
		_isMouseMoveWidget = enable;
	}
	void setMouseMoveWidgetValidRect(const QRect& rect){
		_mouseMoveWidgetValidRect = rect;
	}
	void startFlash()
	{
		QGraphicsColorizeEffect*	effect = new QGraphicsColorizeEffect(_obj);
		QPropertyAnimation*			animation = new QPropertyAnimation(effect, "color", _obj);
		animation->setStartValue(m_flashStartColor);
		animation->setKeyValueAt(0.5, m_flashEndColor);
		animation->setEndValue(m_flashStartColor);
		animation->setDuration(m_flashPeriod * 2);
		_obj->setGraphicsEffect(effect);

		animation->setLoopCount(-1);
		animation->start();
	}
	void stopFlash()
	{
		_obj->setGraphicsEffect(nullptr);
	}
	void setAcceptDragFiles(bool yes)
	{
		m_isAcceptDragFiles = yes;
		_obj->setAcceptDrops(true);
	}
    void setStaysOnTop(bool yes)
    {
       m_staysOnTop=yes;
       bool visible = _obj->isVisible();
       _obj->setWindowFlag(Qt::WindowStaysOnTopHint,m_staysOnTop);
       _obj->setVisible(visible);
    }

private slots:
    void popupMenu(){
        m_menu->popup(QCursor::pos());
    }
protected:
	bool eventFilter(QObject *obj, QEvent *event)
    {
		if (_isMouseMoveWidget){
			static QPoint mousePos;
			static bool mousePressed=false;
			
			if (event->type() == QEvent::MouseButtonPress){
				QPoint objPos = _obj->mapFromGlobal(QCursor::pos());
				if (_mouseMoveWidgetValidRect.contains(objPos)){
					mousePos = QCursor::pos();
					mousePressed = true;
				}
			}
			else if (event->type() == QEvent::MouseButtonRelease){
				mousePressed = false;
			}
			else if (event->type() == QEvent::MouseMove && mousePressed){
					_obj->move(_obj->pos() + QCursor::pos() - mousePos);
					mousePos = QCursor::pos();
			}
			else{
				//return false;
			}
			//return true;
		}
		if (m_isAcceptDragFiles)
		{
			if (event->type() == QEvent::DragEnter)
			{
				QDragEnterEvent* ev = static_cast<QDragEnterEvent*>(event);
				if (ev->mimeData()->hasUrls())
					ev->acceptProposedAction();
			}
			if (event->type() == QEvent::Drop)
			{
				QDropEvent* ev = static_cast<QDropEvent*>(event);
				auto urls = ev->mimeData()->urls();
				if (!urls.isEmpty())
				{
					QStringList files;
					for (const auto& url : urls)
					{
						files.append(url.toLocalFile());
					}
					emit filesDropped(files);
					emit fileDropped(files.at(0));
				}
				return true;
			}
        }
//        if(m_staysOnTop && event->type()==QEvent::Show){
//            //_obj->setWindowFlag(Qt::WindowStaysOnTopHint,true);
//            _obj->setWindowFlags(_obj->windowFlags()|Qt::WindowStaysOnTopHint);
//            return true;
//        }
		return QObject::eventFilter(obj, event);
	}
private:
    XWidgetHelper(QWidget* obj)
        :QObject(obj)
		, _isMouseMoveWidget(false), _obj(obj), m_isAcceptDragFiles(false)
        , m_menu(nullptr)
	{ 
        Q_ASSERT(_obj);
		_obj->installEventFilter(this);
		setFlashColor(Qt::blue, Qt::yellow, 1000);
    }
	static QHash<QWidget*, XWidgetHelper*>* instances(){
		static QHash<QWidget*, XWidgetHelper*> s_instances;
		return &s_instances;
	}
private:
	QWidget*	_obj;
	bool		_isMouseMoveWidget;
	QRect		_mouseMoveWidgetValidRect;
	QPoint		_dragPos;
    QMenu*      m_menu;
	
	QColor		m_flashStartColor;	// 起始颜色
	QColor		m_flashEndColor;	// 结束颜色
	int			m_flashPeriod;		// 闪烁周期

	bool		m_isAcceptDragFiles;
    bool        m_staysOnTop;
};

#define XWIDGET_HELPER(obj)  XWidgetHelper::getInstance(obj)

#endif // _XWIDGET_HELPER

