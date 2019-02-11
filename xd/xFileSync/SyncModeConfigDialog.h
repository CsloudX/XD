#ifndef SYNCMODECONFIGDIALOG_H
#define SYNCMODECONFIGDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QButtonGroup>
#include "AppConfig.h"

namespace Ui {
class SyncModeConfigDialog;
}

class SyncModeConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SyncModeConfigDialog(QWidget *parent = 0);
    ~SyncModeConfigDialog();

    void setSyncOperation(const QMap<FileSync::FileCompareResult,FileSync::SyncOperation>& op);
    QMap<FileSync::FileCompareResult,FileSync::SyncOperation> syncOperation()const;

private slots:
    void setLeftSyncMode(int syncMode);
    void setRightSyncMode(int syncMode);

private:
    void appendSyncOperationComboItem(QComboBox* combo,FileSync::SyncOperation op);

private:
    Ui::SyncModeConfigDialog *ui;
    QButtonGroup* m_btnGroup;
};

#endif // SYNCMODECONFIGDIALOG_H
