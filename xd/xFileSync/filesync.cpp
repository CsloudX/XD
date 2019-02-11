#include "filesync.h"
#include "filesync.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>


FileSync::FileSync(QObject *parent)
    : QObject(parent)
    , m_compareMode(TimeAndSizeCompareMode)
{
}

FileSync::FileCompareResult FileSync::compareFile(const QString &file1, const QString &file2, FileSync::CompareMode mode)
{
    return compareFile(QFileInfo(file1),QFileInfo(file2),mode);
}

void FileSync::compare()
{
    m_wantStop=false;
    emit processed("Start Compare...");
    m_items.clear();
    compareDir(QDir(m_srcDir),QDir(m_dstDir),&m_items);
    emit processed("Compare Done");
}

void FileSync::sync()
{
    m_wantStop = false;
    emit processed("Start Sync...");

    if(!QDir(m_dstDir).exists())
        QDir().mkpath(m_dstDir);

    for(auto& var: m_items)
    {
        emit processed(QString("%1\n%2")
                       .arg(var.srcFile.absoluteFilePath())
                       .arg(var.dstFile.absoluteFilePath()));
        switch (var.op) {
        case NoOperation:break;
        case CopyLeftToRight:
            copyFile(var.srcFile.absoluteFilePath(),var.dstFile.absoluteFilePath());
            break;
        case UpdateRightFromLeft:
            copyFile(var.srcFile.absoluteFilePath(),var.dstFile.absoluteFilePath());
            break;
        case CopyRightToLeft:
            copyFile(var.dstFile.absoluteFilePath(),var.srcFile.absoluteFilePath());
            break;
        case UpdateLeftFromRight:
            copyFile(var.dstFile.absoluteFilePath(),var.srcFile.absoluteFilePath());
            break;
        case DeleteLeft:
            deleteFile(var.srcFile);
            break;
        case DeleteRight:
            deleteFile(var.dstFile);
            break;
        }

        if(m_wantStop)
            return;
    }

    emit processed("Sync Done");
}

void FileSync::getAllFileInfo(const QString &dirStr, QStringList &files)
{
    QDir dir(dirStr);
    files.append(dir.absolutePath());

    auto subFiles = dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    for(auto& var: subFiles)
    {
        if(var.isDir())
            getAllFileInfo(var.absoluteFilePath(),files);
        else
            files.append(var.absoluteFilePath());
    }
}

FileSync::FileCompareResult FileSync::compareFile(const QFileInfo &srcFile, const QFileInfo &dstFile, FileSync::CompareMode mode)
{
    if(!srcFile.exists())   return FileOnlyOnRight;
    if(!dstFile.exists())   return FileOnlyOnLeft;
    if(srcFile.isDir() && dstFile.isDir())  return FileEqual;

    if(srcFile.isFile()&& dstFile.isFile())
    {
        if(srcFile.size()!=dstFile.size()){
            return compareFileWithTime(srcFile,dstFile,NotFixTimeOffset);
        }

        switch (mode) {
        case TimeAndSizeCompareMode:
            return compareFileWithTime(srcFile,dstFile,FixTimeOffset);
            break;
        case ContextCompareMode:
            return compareFileWithContext(srcFile,dstFile);
            break;
        }
    }
    return FileEqual;
}

FileSync::FileCompareResult FileSync::compareFileWithTime(const QFileInfo &srcFile, const QFileInfo &dstFile, FileSync::IsFixTimeOffset fixTimeOffset)
{
    Q_ASSERT(srcFile.isFile() && dstFile.isFile());
    auto srcFileTime = srcFile.lastModified();
    auto dstFileTime = dstFile.lastModified();
    int timeDiff = srcFileTime.secsTo(dstFileTime);
    if(fixTimeOffset==FixTimeOffset && qAbs(timeDiff)<3){
        return FileEqual;
    }
    if(dstFileTime>srcFileTime)
        return FileRightNewer;
    if(dstFileTime<srcFileTime)
        return FileLeftNewer;
    return FileEqual;
}

FileSync::FileCompareResult FileSync::compareFileWithContext(const QFileInfo &srcFile, const QFileInfo &dstFile)
{
    if(isFileContextSame(srcFile.absoluteFilePath(),dstFile.absoluteFilePath()))
        return FileEqual;
    return compareFileWithTime(srcFile,dstFile,NotFixTimeOffset);
}


void FileSync::compareDir(const QDir &dir1, const QDir &dir2, QList<FileSync::Item> *list)
{
    if(m_wantStop) return;

    auto files = dirEntryList(dir1,dir2,m_includeFilter,m_excludeFilter);
    for(auto& var:files){
        QFileInfo file1(dir1.absoluteFilePath(var));
        QFileInfo file2( dir2.absoluteFilePath(var));
        dealCompareFile(file1,file2);
        if(file1.isDir()||file2.isDir())
        {
            compareDir(QDir(file1.absoluteFilePath()),QDir(file2.absoluteFilePath()),list);
        }

        if(m_wantStop) return;
    }
}

void FileSync::dealCompareFile(const QFileInfo &srcFile, const QFileInfo &dstFile)
{
    emit processed(QString("%1\n%2")
                   .arg(srcFile.absoluteFilePath())
                   .arg(dstFile.absoluteFilePath()));

    FileCompareResult fileCompareResult = compareFile(srcFile,dstFile, m_compareMode);
    auto op = syncOperation(fileCompareResult);

    if(op!=NoOperation){
        Item item;
        item.srcFile=srcFile;
        item.dstFile=dstFile;
        item.op=op;
        m_items.append(item);
    }
}

void FileSync::deleteFile(const QFileInfo &file)
{
    if(file.isDir())
    {
        auto files = QDir(file.absoluteFilePath()).entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot|QDir::Hidden);
        for(auto& var:files)
        {
            deleteFile(var);
        }

        QDir(file.path()).rmdir(file.fileName());
    }
    else
        QFile::remove(file.absoluteFilePath());
}

FileSync::SyncOperation FileSync::syncOperation(FileSync::FileCompareResult rs)
{
    auto itr = m_op.find(rs);
    if(itr!=m_op.end())
        return itr.value();
    return NoOperation;
}

void FileSync::printFilesInfo(const QFileInfo &file1, const QFileInfo &file2)
{
    qDebug()<<file1.absoluteFilePath()<<"\t"<<file2.absoluteFilePath();
    qDebug()<<file1.lastModified()<<"\t"<<file2.lastModified();
    qDebug()<<"\r\n";
}

bool FileSync::isFileContextSame(const QString &file1, const QString &file2)
{
    if(QFileInfo(file1).size()!=QFileInfo(file2).size())
        return false;

    QFile f1(file1);
    QFile f2(file2);
    f1.open(QIODevice::ReadOnly);
    f2.open(QIODevice::ReadOnly);
    while(!f1.atEnd()){
        auto d1 = f1.read(1024);
        auto d2 = f2.read(1024);
        if(d1!=d2)
            return false;
    }
    return true;

}

QStringList FileSync::dirEntryList(const QDir &dir, const QStringList &includeFilter, const QStringList &excludeFilter)
{
    QStringList ret, excludes;
    if(includeFilter.isEmpty())
        ret = dir.entryList(QDir::Files|QDir::Dirs| QDir::NoDotAndDotDot|QDir::Hidden);
    else
        ret = dir.entryList(includeFilter,QDir::Files|QDir::Dirs| QDir::NoDotAndDotDot|QDir::Hidden);

    if(!excludeFilter.isEmpty())
        excludes = dir.entryList(excludeFilter,QDir::Files|QDir::Dirs| QDir::NoDotAndDotDot|QDir::Hidden);

    for(auto& var:excludes)
        ret.removeOne(var);
    return ret;
}

QStringList FileSync::dirEntryList(const QDir &srcDir, const QDir &dstDir, const QStringList &includeFilter, const QStringList &excludeFilter)
{
    auto srcFiles = dirEntryList(srcDir,includeFilter,excludeFilter);
    auto dstFiles = dirEntryList(dstDir,includeFilter,excludeFilter);
    for(auto& var:dstFiles){
        if(!srcFiles.contains(var))
            srcFiles.append(var);
    }
    return srcFiles;
}

void FileSync::copyFile(const QFileInfo &srcFile, const QFileInfo &dstFile)
{
    if(dstFile.exists())
        deleteFile(dstFile);

    if(srcFile.isDir())
        QDir().mkdir(dstFile.absoluteFilePath());
    else
        QFile::copy(srcFile.absoluteFilePath(),dstFile.absoluteFilePath());
}
