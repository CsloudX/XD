#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QIcon>
#include <QAbstractButton>
#include <QMenu>
#include <QLineEdit>
#include "AppConfig.h"
#include <Qt/XUnitCalculator.hpp>

namespace Ui {
class MainWindow;
}

class DirSelector:public QObject{
    Q_OBJECT
public:
    DirSelector(QAbstractButton* btn,QLineEdit* edit);

private slots:
    void selectDir();

private:
    QLineEdit* m_edit;
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow* instance(){return s_instance;}

    //AppConfig* config()const{return m_config;}

private slots:
    void compare();
    void sync();
    void appendProcess(const QString& str);
    void exchangeDir();
    void showSrcFile();
    void showDstFile();
    void newConfig();
    void removeConfig();
    void saveConfig();
    void updateConfigList();
    void updateConfigItem();
    void showFilterDialog();
    void changeSyncMode();

private:
    void setFileInfoToTable(int row,int col,const QFileInfo& file,const QString& rootDir);
    AppConfig::Item currentConfigItem()const;
    void setConfigItem(const AppConfig::Item& item);

protected:
    void paintEvent(QPaintEvent *);


private:
    Ui::MainWindow *ui;
    AppConfig* m_config;
    FileSync* m_fileSync;
    XUnitCalculator m_unit;

    QIcon m_iconLeft;
    QIcon m_iconRight;
    QIcon m_iconDelete;

    QMenu* m_tableMenu;
    QMap<FileSync::FileCompareResult,FileSync::SyncOperation> m_op;
    QStringList m_includeFilter;
    QStringList m_excludeFilter;

    static MainWindow* s_instance;
};

#endif // MAINWINDOW_H
