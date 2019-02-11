#include "SyncModeConfigDialog.h"
#include "ui_SyncModeConfigDialog.h"
#include <QButtonGroup>
#include <Qt/XComboBoxHelper.h>
#include "IconMgr.h"


static IconMgr ICON;

SyncModeConfigDialog::SyncModeConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SyncModeConfigDialog)
{
    ui->setupUi(this);

    appendSyncOperationComboItem(ui->OnlyLeftCombo,FileSync::CopyLeftToRight);
    appendSyncOperationComboItem(ui->OnlyLeftCombo,FileSync::DeleteLeft);
    appendSyncOperationComboItem(ui->OnlyLeftCombo,FileSync::NoOperation);

    appendSyncOperationComboItem(ui->OnlyRightCombo,FileSync::CopyRightToLeft);
    appendSyncOperationComboItem(ui->OnlyRightCombo,FileSync::DeleteRight);
    appendSyncOperationComboItem(ui->OnlyRightCombo,FileSync::NoOperation);

    appendSyncOperationComboItem(ui->LeftNewCombo,FileSync::UpdateRightFromLeft);
    appendSyncOperationComboItem(ui->LeftNewCombo,FileSync::UpdateLeftFromRight);
    appendSyncOperationComboItem(ui->LeftNewCombo,FileSync::NoOperation);

    appendSyncOperationComboItem(ui->RightNewCombo,FileSync::UpdateLeftFromRight);
    appendSyncOperationComboItem(ui->RightNewCombo,FileSync::UpdateRightFromLeft);
    appendSyncOperationComboItem(ui->RightNewCombo,FileSync::NoOperation);

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->addButton(ui->twoWayBtn,TwoWaySyncMode);
    m_btnGroup->addButton(ui->mirrorBtn,MirrorSyncMode);
    m_btnGroup->addButton(ui->updateBtn,UpdateSyncMode);
    m_btnGroup->addButton(ui->customBtn,CustomSyncMode);

    connect(m_btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(setRightSyncMode(int)));
   // connect(this,SINGAL(rightSyncModeChanged(int)),this,SLOT(setLeftSyncMode(int)));
}

SyncModeConfigDialog::~SyncModeConfigDialog()
{
    delete ui;
}

void SyncModeConfigDialog::setSyncOperation(const QMap<FileSync::FileCompareResult, FileSync::SyncOperation> &op)
{
   XCOMBO_BOX_HELPER(ui->OnlyLeftCombo)->setCurrentData(op[FileSync::FileOnlyOnLeft]);
   XCOMBO_BOX_HELPER(ui->OnlyRightCombo)->setCurrentData(op[FileSync::FileOnlyOnRight]);
   XCOMBO_BOX_HELPER(ui->LeftNewCombo)->setCurrentData(op[FileSync::FileLeftNewer]);
   XCOMBO_BOX_HELPER(ui->RightNewCombo)->setCurrentData(op[FileSync::FileRightNewer]);
}

QMap<FileSync::FileCompareResult, FileSync::SyncOperation> SyncModeConfigDialog::syncOperation() const
{
    QMap<FileSync::FileCompareResult, FileSync::SyncOperation> ret;
    ret[FileSync::FileOnlyOnLeft]= (FileSync::SyncOperation)ui->OnlyLeftCombo->currentData().toInt();
    ret[FileSync::FileOnlyOnRight]= (FileSync::SyncOperation)ui->OnlyRightCombo->currentData().toInt();
    ret[FileSync::FileLeftNewer]= (FileSync::SyncOperation)ui->LeftNewCombo->currentData().toInt();
    ret[FileSync::FileRightNewer]= (FileSync::SyncOperation)ui->RightNewCombo->currentData().toInt();
    return ret;
}

void SyncModeConfigDialog::setLeftSyncMode(int syncMode)
{
    m_btnGroup->button(syncMode)->setChecked(true);
}

void SyncModeConfigDialog::setRightSyncMode(int syncMode)
{
    if(syncMode!=CustomSyncMode)
        setSyncOperation(AppConfig::syncOperation((SyncMode)syncMode));
}

void SyncModeConfigDialog::appendSyncOperationComboItem(QComboBox *combo, FileSync::SyncOperation op)
{

    QIcon ico;
    QString text;
    switch (op) {
    case FileSync::CopyLeftToRight:
        ico = ICON.RIGHT_ADD();
        text=tr("Copy Left to Right");
        break;
    case FileSync::CopyRightToLeft:
        ico = ICON.LEFT_ADD();
        text=tr("Copy Right to Left");
        break;
    case FileSync::UpdateLeftFromRight:
        ico = ICON.LEFT();
        text=tr("Update Left from Right");
        break;
    case FileSync::UpdateRightFromLeft:
        ico = ICON.RIGHT();
        text=tr("Update Right from Left");
        break;
    case FileSync::DeleteLeft:
        ico = ICON.LEFT_DEL();
        text=tr("Delete Left");
        break;
    case FileSync::DeleteRight:
        ico = ICON.RIGHT_DEL();
        text=tr("Delete Right");
        break;
    case FileSync::NoOperation:
        ico = ICON.NO_OP();
        text=tr("No Operation");
        break;
    default:
        break;
    }
    combo->addItem(ico,text,op);
}
