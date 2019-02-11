#ifndef _XCOMBO_BOX_HELPER_H
#define _XCOMBO_BOX_HELPER_H

#include <QComboBox>
#include <QHash>
#include <QPainter>
#include <QColorDialog>

class XComboBoxHelper : public QObject{
	Q_OBJECT

signals:
	void activated(const QColor& color);
	void highlighted(const QColor& color);

public:
	enum Mode{NoMode,ColorMode};

	~XComboBoxHelper(){instances()->remove(m_obj);}
	static XComboBoxHelper* getInstance(QComboBox* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XComboBoxHelper(obj);
		}
		return (*instances())[obj];
	}

	void setMode(Mode mode)
	{
		Q_ASSERT(m_mode == NoMode);
		m_mode = mode;
		if (m_mode == ColorMode) {
			m_obj->addItem(colorIcon(QColor()), tr("Custom"));
		}
	}

	void addItem(const QColor& color, const QString& name)
	{
		Q_ASSERT(m_mode == ColorMode);
		int customIndex = m_obj->findData(QVariant());
		m_obj->insertItem(customIndex, colorIcon(color), name, color);
	}

	void setCurrentData(const QVariant& data)
	{
		int index = m_obj->findData(data);
		Q_ASSERT(index != -1);
		m_obj->setCurrentIndex(index);
	}

	QColor currentColor()const
	{
		Q_ASSERT(m_mode == ColorMode);

		int index = m_obj->currentIndex();
		int customIndex = m_obj->findData(QVariant());
		if (index == customIndex)
			return m_obj->itemData(customIndex, Qt::UserRole + 1).value<QColor>();
		else
			return m_obj->currentData().value<QColor>();
	}

public slots:
	void setColor(const QColor& color)
	{
		Q_ASSERT(m_mode == ColorMode);
		auto index = m_obj->findData(color);
		if (index == -1) {
			int customIndex = m_obj->findData(QVariant());
			m_obj->setItemIcon(customIndex, colorIcon(color));
			m_obj->setItemData(customIndex, color, Qt::UserRole + 1);
			m_obj->setCurrentIndex(index);
		}
		else {
			m_obj->setCurrentIndex(index);
		}
	}

private slots:
	void onActivated(int index)
	{
		if (m_mode == ColorMode) {
			int customIndex = m_obj->findData(QVariant());
			if (customIndex == index) {
				QColor color = m_obj->itemData(customIndex, Qt::UserRole + 1).value<QColor>();
				color = QColorDialog::getColor(color);
				if (color.isValid()) {
					m_obj->setItemIcon(customIndex, colorIcon(color));
					m_obj->setItemData(customIndex, color, Qt::UserRole + 1);
					emit activated(color);
				}
			}
			else {
				emit activated(m_obj->itemData(index).value<QColor>());
			}
		}
	}

private:
	static QIcon colorIcon(const QColor& color)
	{
		QPixmap pixmap(32, 32);
		if(color.isValid())
			pixmap.fill(color);
		QPainter painter(&pixmap);
		painter.drawRect(QRect(QPoint(0,0),pixmap.size()-QSize(1,1)));
		return pixmap;
	}

    XComboBoxHelper(QComboBox* obj) :QObject(obj), m_obj(obj),m_mode(NoMode){ 
        Q_ASSERT(m_obj);
		connect(m_obj, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
    }
	static QHash<QComboBox*, XComboBoxHelper*>* instances(){
		static QHash<QComboBox*, XComboBoxHelper*> s_instances;
		return &s_instances;
	}
private:
	QComboBox*	m_obj;
	Mode		m_mode;
};

#define XCOMBO_BOX_HELPER(obj)  XComboBoxHelper::getInstance(obj)

#endif // _XCOMBO_BOX_HELPER
