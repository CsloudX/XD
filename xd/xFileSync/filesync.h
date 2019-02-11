#ifndef FILESYNC_H
#define FILESYNC_H

#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <QPair>

class FileSync : public QObject
{
    Q_OBJECT

signals:
    void processed(const QString& str);

public:
    enum CompareMode{
        TimeAndSizeCompareMode,
        ContextCompareMode
    };
    enum SyncOperation{
        NoOperation,
        CopyLeftToRight,
        UpdateRightFromLeft,
        CopyRightToLeft,
        UpdateLeftFromRight,
        DeleteLeft,
        DeleteRight
    };

    enum FileCompareResult{
        FileEqual,
        FileOnlyOnLeft,
        FileOnlyOnRight,
        FileLeftNewer,
        FileRightNewer
    };



    enum IsFixTimeOffset{NotFixTimeOffset, FixTimeOffset};
    typedef struct{
        QFileInfo srcFile;
        QFileInfo dstFile;
        SyncOperation op;
    }Item;

    explicit FileSync(QObject *parent = nullptr);

public:
    const QList<Item>& items()const{return m_items;}
    static FileCompareResult compareFile(const QString& file1, const QString& file2,CompareMode mode);
    void setSyncOperation(FileCompareResult rs,SyncOperation op){m_op[rs]=op;}
    void setSyncOperation(const QMap<FileCompareResult,SyncOperation>& op){m_op=op;}

public slots:
    void setSrcDir(const QString& dir){m_srcDir=dir;}
    void setDstDir(const QString& dir){m_dstDir=dir;}
    void setCompareMode( CompareMode mode){m_compareMode=mode;}
    void setIncludeFilter(const QStringList& filter){m_includeFilter=filter;}
    void setExcludeFitler(const QStringList& filter){m_excludeFilter=filter;}
    void compare();
    void sync();
    void stop(){m_wantStop=true;}

private:
    static void getAllFileInfo(const QString& dir, QStringList& files);
    static FileCompareResult compareFile(const QFileInfo& srcFile, const QFileInfo& dstFile,CompareMode mode);
    static FileCompareResult compareFileWithTime(const QFileInfo& srcFile, const QFileInfo& dstFile
                                                 ,IsFixTimeOffset fixTimeOffset=NotFixTimeOffset);
    static FileCompareResult compareFileWithContext(const QFileInfo& srcFile, const QFileInfo& dstFile);
    void compareDir(const QDir& dir1,const QDir& dir2, QList<Item>* list);
    void dealCompareFile(const QFileInfo& file1, const QFileInfo& file2);
    void copyFile(const QFileInfo& srcFile,const QFileInfo& dstFile);
    void deleteFile(const QFileInfo& file);
    SyncOperation syncOperation(FileCompareResult rs);
    static void printFilesInfo(const QFileInfo& file1,const QFileInfo& file2);
    static bool isFileContextSame(const QString& file1,const QString& file2);
    static QStringList dirEntryList(const QDir& dir,const QStringList& includeFilter,const QStringList& excludeFilter);
    static QStringList dirEntryList(const QDir& srcDir,const QDir& dstDir,
                                    const QStringList& includeFilter,const QStringList& excludeFilter);

private:
    QString m_srcDir;
    QString m_dstDir;
    CompareMode m_compareMode;
    QMap<FileCompareResult,SyncOperation> m_op;
    QStringList  m_includeFilter;
    QStringList  m_excludeFilter;
    QList<Item> m_items;
    bool m_wantStop;
};

#endif // FILESYNC_H
