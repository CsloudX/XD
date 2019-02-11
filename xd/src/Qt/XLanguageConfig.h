#ifndef _X_LANGUAGE_CONFIG_H
#define _X_LANGUAGE_CONFIG_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QList>
#include <QTranslator>
#include <QApplication>

class XLanguageConfig: public QObject
{
	Q_OBJECT
signals:
	void languageChanged(const QString& language);

public:
	XLanguageConfig(QObject* parent=nullptr)
		:QObject(parent), m_dir("./language")
	{}
	~XLanguageConfig(){ removeAllTranlator(); }

	QStringList languages()const { return QDir(m_dir).entryList(QDir::Dirs | QDir::NoDotAndDotDot); }
	QString currentLanguage()const{ return m_currentLanguage; }
	void setDefaultLanguage(const QString& name)
	{
		QDir dir(m_dir);
		dir.mkdir(name);
	}

public slots:
	void setLanguage(const QString& language)
	{
		if (m_currentLanguage != language)
		{
			loadAllQmFile(m_dir);
			loadAllQmFile(QString("%1/%2").arg(m_dir).arg(language));

			m_currentLanguage = language;
			emit languageChanged(m_currentLanguage);
		}
	}
	void setDir(const QString& dir){ m_dir = dir; }

private:
	void loadAllQmFile(const QString& dir) 
	{
		QStringList files = QDir(dir).entryList(QStringList() << "*.qm", QDir::Files);
		for (auto& var : files)
		{
			QString file(QString("%1/%2").arg(dir).arg(var));
			QTranslator* translator = new QTranslator(qApp);
			translator->load(file);
			qApp->installTranslator(translator);

			m_tranlators.append(translator);
		}
	}
	void removeAllTranlator()
	{
		for (auto& t : m_tranlators)
		{
			qApp->removeTranslator(t);
		}
		qDeleteAll(m_tranlators);
	}

private:
	QString m_dir;
	QString m_currentLanguage;
	QList<QTranslator*> m_tranlators;
};


#endif //_X_LANGUAGE_CONFIG_H
