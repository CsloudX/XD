#ifndef _X_SKIN_CONFIG_H
#define _X_SKIN_CONFIG_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QApplication>

class XSkinConfig: public QObject
{
	Q_OBJECT

signals:
	void skinChanged(const QString& skin);

public:
	XSkinConfig(QObject* parent)
		:QObject(parent), m_dir("./skin")
	{}

	QStringList skins()const { return QDir(m_dir).entryList(QDir::Dirs | QDir::NoDotAndDotDot); }
	
	QString currentSkin()const{ return m_currentSkin; }

public slots:
	void setSkin(const QString& skin)
	{
		QString qss;
		loadAllQssFile(m_dir, qss);
		loadAllQssFile(QString("%1/%2").arg(m_dir).arg(skin), qss);
		qApp->setStyleSheet(qss);

		m_currentSkin = skin;
		emit skinChanged(m_currentSkin);
	}

	void setDir(const QString& dir){ m_dir = dir; }

private:
	void loadAllQssFile(const QString& dir, QString& qss) const
	{
		QStringList files = QDir(dir).entryList(QStringList() << "*.qss", QDir::Files);
		for (auto& var : files)
		{
			QFile file(QString("%1/%2").arg(dir).arg(var));
			if (file.open(QIODevice::ReadOnly))
			{
				qss.append(file.readAll());
			}
		}
	}

private:
	QString m_dir;
	QString m_currentSkin;
};


#endif //_X_SKIN_CONFIG_H
