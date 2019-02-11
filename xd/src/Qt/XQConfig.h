#ifndef _X_Q_CONFIG_H
#define _X_Q_CONFIG_H

#include <QSettings>

/************ Example ***********
#define APP_CONFIG_LIST(_) \
	_(ID			,0) \
	_(Name			,"") 
X_DEF_CONFIG(App,"App.ini", APP_CONFIG_LIST)
or
X_DEF_CONFIG_EX(App, APP_CONFIG_LIST)
*********************************/

#define _X_DEF_CONFIG_SET(key,def) void set##key(const QVariant& value){setValue(#key,value);}
#define _X_DEF_CONFIG_GET(key,def) QVariant key()const{return value(#key,def);}
#define _X_DEF_CONFIG_NAME(key,def) static QString key##Name(){ return #key;}
#define  X_DEF_CONFIG(Name,FileName,List) \
	class Name##Config : public XQConfig{ \
    public:Name##Config(QObject* parent=nullptr):XQConfig(#FileName".ini",parent){} \
    List(_X_DEF_CONFIG_NAME) \
    List(_X_DEF_CONFIG_GET) \
    List(_X_DEF_CONFIG_SET) };

#define X_DEF_CONFIG_EX(Name,List) X_DEF_CONFIG(Name,Name,List)


class XQConfig : public QObject
{
	Q_OBJECT
	
signals:
	void valueChanged(const QString& key,const QVariant& value);
	
public:
	XQConfig(const QString& fileName, QObject* parent = nullptr)
		:QObject(parent)
        ,m_settings(new QSettings(fileName,QSettings::IniFormat,this))
		{}
	virtual ~XQConfig(){}
	
protected:
	QVariant value(const QString& key,const QVariant& defaultValue=QVariant())const
	{ return m_settings->value(key,defaultValue);}
	void setValue(const QString& key,const QVariant& value)
	{
		if(m_settings->value(key)!=value)
		{
			m_settings->setValue(key,value);
			emit valueChanged(key,value);
		}
	}
private:
	QSettings* m_settings;
};

#endif //_X_Q_CONFIG_H
