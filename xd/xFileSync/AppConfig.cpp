#include "AppConfig.h"
#include <XXml.hpp>

#define XML_NAME    "xFileSync.xml"
#define S_DEF_ITEM  "DefaultItem"
#define S_NAME  "name"
#define SRC_DIR "SrcDir"
#define DST_DIR "DstDir"
#define COMPARE_MODE "CompareMode"
#define SYNC_OP "SyncOp"
#define INCLUDE_FILTER "Include"
#define EXCLUDE_FILTER "Exclude"
#define S_KEY   "key"
#define S_VALUE "value"
#define S_ITEM  "item"

QMap<FileSync::FileCompareResult, FileSync::SyncOperation> s_twoWaySyncMode;
QMap<FileSync::FileCompareResult, FileSync::SyncOperation> s_updateSyncMode;
QMap<FileSync::FileCompareResult, FileSync::SyncOperation> s_mirrorSyncMode;
QMap<FileSync::FileCompareResult, FileSync::SyncOperation> s_invalidSyncMode;

AppConfig::AppConfig(QObject *parent)
    : QObject(parent),m_needSave(false)
{
    load();
}

AppConfig::~AppConfig()
{
    maybeSave();
}

void AppConfig::removeItem(const QString &name)
{
    m_items.remove(name);
    m_needSave=true;
}

QStringList AppConfig::allItems() const
{
    return m_items.keys();
}

void AppConfig::getItem(const QString &name, AppConfig::Item *item)
{
    *item = m_items[name];
}

void AppConfig::setItem(const QString& name,const AppConfig::Item &item)
{
    m_items[name]=item;
    m_needSave=true;
}

void AppConfig::setDefaultItem(const QString &name)
{
    if(m_defaultItemName!=name)
    {
        m_defaultItemName=name;
        m_needSave=true;
    }
}


SyncMode AppConfig::syncMode(const QMap<FileSync::FileCompareResult, FileSync::SyncOperation> &op)
{
    if(op==twoWaySyncMode())
        return TwoWaySyncMode;
    else if (op==updateSyncMode())
        return UpdateSyncMode;
    else if(op==mirrorSyncMode())
        return MirrorSyncMode;
    return CustomSyncMode;
}

const QMap<FileSync::FileCompareResult, FileSync::SyncOperation>& AppConfig::syncOperation(SyncMode syncMode)
{
    switch (syncMode) {
    case TwoWaySyncMode:return twoWaySyncMode();
    case UpdateSyncMode:return updateSyncMode();
    case MirrorSyncMode:return mirrorSyncMode();
    default: break;
    }
    Q_ASSERT(false);
    return s_invalidSyncMode;
}

void AppConfig::load()
{
    XXml xml;
    xml.load(XML_NAME);
    m_defaultItemName = QString::fromStdWString(xml.data().attr(S_DEF_ITEM));
    for(auto& node:xml.children())
    {
        auto item = &m_items[QString::fromStdWString(node->data().attr(S_NAME))];
        item->srcDir = QString::fromStdWString(node->data().attr(SRC_DIR));
        item->dstDir = QString::fromStdWString(node->data().attr(DST_DIR));
        item->compareMode = (FileSync::CompareMode)node->data().attr(COMPARE_MODE).toInt();
        QString str = QString::fromStdWString(node->data().attr(INCLUDE_FILTER));
        if(!str.isEmpty())   item->includeFilter = str.split(";");
        str = QString::fromStdWString(node->data().attr(EXCLUDE_FILTER));
        if(!str.isEmpty())   item->excludeFilter = str.split(";");
        auto nOP = node->findChild(SYNC_OP);
        if(nOP){
            for(auto& op:nOP->children())
            {
                item->op[(FileSync::FileCompareResult)op->data().attr(S_KEY).toInt()]
                        =(FileSync::SyncOperation)op->data().attr(S_VALUE).toInt();
            }
        }
    }
    m_needSave=false;
}

void AppConfig::save()
{
    XXml xml;
    xml.data().key=L"xFileSync";
    xml.data().setAttr(S_DEF_ITEM,m_defaultItemName.toStdWString());
    QMapIterator<QString,Item> i(m_items);
    while (i.hasNext()) {
        i.next();
        auto node = xml.createSubNode(S_ITEM);
        node->data().setAttr(S_NAME,i.key().toStdWString());
        Item item = i.value();
        node->data().setAttr(SRC_DIR,item.srcDir.toStdWString());
        node->data().setAttr(DST_DIR,item.dstDir.toStdWString());
        node->data().setAttr(COMPARE_MODE,item.compareMode);
        node->data().setAttr(INCLUDE_FILTER, item.includeFilter.join(";").toStdString());
        node->data().setAttr(EXCLUDE_FILTER, item.excludeFilter.join(";").toStdString());
        auto nOP = node->createSubNode(SYNC_OP);
        QMapIterator<FileSync::FileCompareResult,FileSync::SyncOperation> iOP(item.op);
        while(iOP.hasNext())
        {
            iOP.next();
            auto n=nOP->createSubNode(S_ITEM);
            n->data().setAttr(S_KEY,iOP.key());
            n->data().setAttr(S_VALUE,iOP.value());
        }
    }

    xml.save(XML_NAME);
    m_needSave=false;
}

const QMap<FileSync::FileCompareResult, FileSync::SyncOperation> &AppConfig::twoWaySyncMode()
{
    auto& op = s_twoWaySyncMode;
    if(op.isEmpty()){
        op[FileSync::FileOnlyOnLeft]=FileSync::CopyLeftToRight;
        op[FileSync::FileOnlyOnRight]=FileSync::CopyRightToLeft;
        op[FileSync::FileLeftNewer]=FileSync::UpdateRightFromLeft;
        op[FileSync::FileRightNewer]=FileSync::UpdateLeftFromRight;
    }
    return op;
}

const QMap<FileSync::FileCompareResult, FileSync::SyncOperation> &AppConfig::updateSyncMode()
{
    auto& op = s_updateSyncMode;
    if(op.isEmpty()){
        op[FileSync::FileOnlyOnLeft]=FileSync::CopyLeftToRight;
        op[FileSync::FileOnlyOnRight]=FileSync::NoOperation;
        op[FileSync::FileLeftNewer]=FileSync::UpdateRightFromLeft;
        op[FileSync::FileRightNewer]=FileSync::NoOperation;
    }
    return op;
}

const QMap<FileSync::FileCompareResult, FileSync::SyncOperation> &AppConfig::mirrorSyncMode()
{
    auto& op = s_mirrorSyncMode;
    if(op.isEmpty()){
        op[FileSync::FileOnlyOnLeft]=FileSync::CopyLeftToRight;
        op[FileSync::FileOnlyOnRight]=FileSync::DeleteRight;
        op[FileSync::FileLeftNewer]=FileSync::UpdateRightFromLeft;
        op[FileSync::FileRightNewer]=FileSync::UpdateRightFromLeft;
    }
    return op;
}
