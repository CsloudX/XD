#ifndef _X_ICON_MANAGER_H
#define _X_ICON_MANAGER_H

#include <QIcon>
#include <QPainter>
#include "../XGlobal.h"

/**
class XIconManager {
public:
	DEF_XICON(ICO1, "E:/1.png")
	DEF_XICON(ICO2, "E:/2.png")
	DEF_XICO2(ICO3,ICO1,ICO2)
	DEF_XICO2_EX(ICO4, ICO1, ICO2,Qt::AlignRight|Qt::AlignBottom)
};
*/

#define DEF_XICON(name,fileName) const XIcon& name##(){static XIcon icon = XIcon(#name,fileName);return icon;}
#define DEF_XICO2(name,ico,merge) const XIcon& name##(){static XIcon icon=XIcon(#name,ico##(),merge##()); return icon;}
#define DEF_XICO2_EX(name,ico,merge,align) const XIcon& name##(){static XIcon icon=XIcon(#name,ico##(),merge##(),align); return icon;}

class XIcon
{
public:
	XIcon(const QString& name, const QString& fileName,const XIcon* merge=nullptr,Qt::Alignment align=0) 
		:m_name(name), m_fileName(fileName),m_merge(merge),m_align(align){}
	XIcon(const QString& name, const XIcon& other, const XIcon& merge, Qt::Alignment align = 0)
		:m_name(name), m_fileName(other.fileName()), m_merge(&merge), m_align(align) {}
	QIcon toIcon()const{ return QIcon(toPixmap()); }
	QPixmap toPixmap()const
	{
		QPixmap pixmap(m_fileName);
		if (m_merge)
		{
			auto mergePixmap = m_merge->toPixmap();
			auto baseSize = pixmap.size();
			auto mergeSize = mergePixmap.size();

			Q_ASSERT(mergeSize.width() <= baseSize.width() && mergeSize.height() <= baseSize.height());

			int x = 0;
			int y = 0;
			if (m_align&Qt::AlignRight) {
				x = baseSize.width() - mergeSize.width();
			}
			else if (m_align&Qt::AlignHCenter) {
				x = (baseSize.width() - mergeSize.width()) / 2;
			}
			if (m_align&Qt::AlignBottom) {
				y = baseSize.height() - mergeSize.height();
			}
			else if (m_align&Qt::AlignHCenter) {
				y = (baseSize.height() - mergeSize.height()) / 2;
			}

			QPainter painter(&pixmap);
			painter.drawPixmap(x, y, m_merge->toPixmap());
		}
		return pixmap;
	}

	const QString& name()const{ return m_name; }
	const QString& fileName()const{ return m_fileName; }

	operator QIcon()const{ return toIcon(); }

private:
	QString m_name;
	QString m_fileName;
	const XIcon* m_merge;
	Qt::Alignment m_align;
};


#endif //_X_ICON_MANAGER_H
