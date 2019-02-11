#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QSettings>
#include <QMap>
#include "filesync.h"

enum SyncMode{
  CustomSyncMode,
  TwoWaySyncMode,
  UpdateSyncMode,
  MirrorSyncMode
};

class AppConfig : private QObject
{
    Q_OBJECT
public:
    typedef struct{
        QString srcDir;
        QString dstDir;
        FileSync::CompareMode compareMode;
        QMap<FileSync::FileCompareResult,FileSync::SyncOperation> op;
        QStringList includeFilter;
        QStringList excludeFilter;
    }Item;
    explicit AppConfig(QObject *parent = nullptr);
    ~AppConfig();

public slots:
    void removeItem(const QString& name);

public:
    QStringList allItems()const;
    void getItem(const QString& name,Item* item) ;
    void setItem(const QString& name,const Item& item);
    const QString& defaultItem()const{return m_defaultItemName;}
    void setDefaultItem(const QString& name);

    static SyncMode syncMode(const QMap<FileSync::FileCompareResult,FileSync::SyncOperation>& op);
    static const QMap<FileSync::FileCompareResult,FileSync::SyncOperation>& syncOperation(SyncMode syncMode);

private:
    void load();
    void save();
    void maybeSave(){if(m_needSave) save();}
    static const QMap<FileSync::FileCompareResult, FileSync::SyncOperation> &twoWaySyncMode();
    static const QMap<FileSync::FileCompareResult,FileSync::SyncOperation>& updateSyncMode();
    static const QMap<FileSync::FileCompareResult,FileSync::SyncOperation>& mirrorSyncMode();

private:
    QMap<QString, Item> m_items;
    QString m_defaultItemName;
    bool m_needSave;
};

#endif // APPCONFIG_H
