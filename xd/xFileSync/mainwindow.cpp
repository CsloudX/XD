#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <Qt/QHolder.hpp>
#include <QDebug>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QTime>
#include <QDesktopServices>
#include <QCompleter>
#include <QDirModel>
#include <QInputDialog>
#include "FilterDialog.h"
#include "SyncModeConfigDialog.h"
#include <Qt/XComboBoxHelper.h>
#include <Qt/XTableWidgetHelper.h>
#include <Qt/XWidgetHelper.h>
#include <Qt/XCustomWindowHelper.h>
#include "IconMgr.h"

static IconMgr ICON;

MainWindow* MainWindow::s_instance = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    XCUSTOM_WINDOW_HELPER(this)->titleBar()->midLayout()
            ->addWidget(new QLabel("By XD(QQ:978028760)"));
    XCUSTOM_WINDOW_HELPER(this)->titleBar()->midLayout()->addStretch();
    setAttribute(Qt::WA_TranslucentBackground);

    Q_ASSERT(s_instance == nullptr);
    s_instance = this;

    m_fileSync = new FileSync(this);
    connect(m_fileSync,SIGNAL(processed(QString)),this,SLOT(appendProcess(QString)));

    ui->compareModeCombo->addItem(tr("Time & Size"),FileSync::TimeAndSizeCompareMode);
    ui->compareModeCombo->addItem(tr("Context"),FileSync::ContextCompareMode);

    ui->syncModeCombo->addItem(tr("Two Way"),TwoWaySyncMode);
    ui->syncModeCombo->addItem(tr("Mirror"),MirrorSyncMode);
    ui->syncModeCombo->addItem(tr("Update"),UpdateSyncMode);
    ui->syncModeCombo->addItem(tr("Custom"),CustomSyncMode);
    connect(ui->syncModeCombo,SIGNAL(activated(int)),this,SLOT(changeSyncMode()));

    m_config = new AppConfig(this);
    connect(ui->newCfgButton,SIGNAL(clicked(bool)),this,SLOT(newConfig()));
    connect(ui->saveCfgButton,SIGNAL(clicked(bool)),this,SLOT(saveConfig()));
    connect(ui->delCfgButton,SIGNAL(clicked(bool)),this,SLOT(removeConfig()));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateConfigItem()));
    updateConfigList();
//    AppConfig::Item item;
//    m_config->getDefaultItem(&item);
//    setConfigItem(item);

    connect(ui->compareButton,SIGNAL(clicked()),this,SLOT(compare()));
    connect(ui->syncButton,SIGNAL(clicked()),this,SLOT(sync()));

    ui->tableWidget->setColumnWidth(2,25);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Fixed);

    new DirSelector(ui->srcDirBtn,ui->srcDirEdit);
    new DirSelector(ui->dstDirBtn,ui->dstDirEdit);

    connect(ui->exchangeDirBtn,SIGNAL(clicked(bool)),this,SLOT(exchangeDir()));


    XUnitCalculator::UnitItem B = {1,"B"};
    XUnitCalculator::UnitItem KB = {B.size*1024,"KB"};
    XUnitCalculator::UnitItem MB = {KB.size*1024,"MB"};
    m_unit.appendUnit(B);
    m_unit.appendUnit(KB);
    m_unit.appendUnit(MB);

    ui->tableWidget->setTextElideMode(Qt::ElideMiddle);

    m_iconLeft = QIcon(":/res/left.png");
    m_iconRight  =  QIcon(":/res/right.png");
    m_iconDelete  =  QIcon(":/res/delete.png");

    connect(ui->stopButton,SIGNAL(clicked(bool)),m_fileSync,SLOT(stop()));

    m_tableMenu = new QMenu(this);
    m_tableMenu->addAction(tr("Show Src File"),this,SLOT(showSrcFile()));
    m_tableMenu->addAction(tr("Show Dst File"),this,SLOT(showDstFile()));
    XWIDGET_HELPER(ui->tableWidget)->setMenu(m_tableMenu);

    connect(ui->filterBtn,SIGNAL(clicked(bool)),this,SLOT(showFilterDialog()));

    //QTimer::singleShot(100,this,SLOT(compare()));
#ifndef QT_NO_DEBUG
    //auto rs = FileSync::compareFile("E:/_proj/xd/bin/xFileSync.exe","I:/xd/bin/xFileSync.exe",FileSync::TimeAndSizeCompareMode);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::compare()
{
    auto configItem = currentConfigItem();
    if(configItem.srcDir.isEmpty()){
        QMessageBox::information(this,"",tr("Src Dir is Empty!"));
        return;
    }
    if(configItem.dstDir.isEmpty()){
        QMessageBox::information(this,"",tr("Dst Dir is Empty!"));
        return;
    }
    if(configItem.srcDir==configItem.dstDir){
        QMessageBox::information(this,"",tr("Src Dir is equal Dst Dir!"));
        return;
    }

    if(configItem.srcDir.startsWith(configItem.dstDir) || configItem.dstDir.startsWith(configItem.srcDir)){
        QMessageBox::information(this,"",tr("Invalid Compare Dirs!"));
        return;
    }

    m_fileSync->setSrcDir(configItem.srcDir);
    m_fileSync->setDstDir(configItem.dstDir);
    m_fileSync->setCompareMode(configItem.compareMode);
    m_fileSync->setSyncOperation(configItem.op);
    m_fileSync->setIncludeFilter(configItem.includeFilter);
    m_fileSync->setExcludeFitler(m_excludeFilter);

    ui->progressBar->setRange(0,0);
    XWidgetDisabler disabler1(ui->compareButton);
    XWidgetDisabler disabler2(ui->syncButton);
    QCoreApplication::processEvents();
    m_fileSync->compare();

    auto& items  = m_fileSync->items();
    int count = items.count();
    ui->tableWidget->setRowCount(count);
    for(int i=0;i<count;++i){
        auto& var = items.at(i);
        setFileInfoToTable(i,0,var.srcFile,configItem.srcDir);
        setFileInfoToTable(i,3,var.dstFile,configItem.dstDir);

        auto item = XTABLE_WIDGET_HELPER(ui->tableWidget)->item(i,2);
        item->setTextAlignment(Qt::AlignCenter);
        QIcon ico;
        switch (var.op) {
        case FileSync::NoOperation:
            ico = ICON.NO_OP();
            break;
        case FileSync::CopyLeftToRight:
            ico = ICON.RIGHT_ADD();
            break;
        case FileSync::CopyRightToLeft:
            ico = ICON.LEFT_ADD();
            break;
        case FileSync::UpdateRightFromLeft:
            ico = ICON.RIGHT();
            break;
        case FileSync::UpdateLeftFromRight:
            ico = ICON.LEFT();
            break;
        default:
            break;
        }
        item->setIcon(ico);
    }
    ui->progressBar->setRange(0,100);
}

void MainWindow::sync()
{
    ui->progressBar->setRange(0,0);
    XWidgetDisabler disabler1(ui->compareButton);
    XWidgetDisabler disabler2(ui->syncButton);
    QCoreApplication::processEvents();
    m_fileSync->sync();
    compare();
    ui->progressBar->setRange(0,100);
}

void MainWindow::appendProcess(const QString &str)
{
    ui->logLabel->setText(str);
    QCoreApplication::processEvents();
}

void MainWindow::exchangeDir()
{
    QString srcDir=ui->srcDirEdit->text();
    QString dstDir = ui->dstDirEdit->text();
    ui->srcDirEdit->setText(dstDir);
    ui->dstDirEdit->setText(srcDir);
}

void MainWindow::showSrcFile()
{
    int row = ui->tableWidget->currentRow();
    if(row!=-1)
    {
        QUrl url = QUrl::fromLocalFile(ui->tableWidget->item(row,0)->toolTip());
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::showDstFile()
{
    int row = ui->tableWidget->currentRow();
    if(row!=-1)
    {
        QUrl url = QUrl::fromLocalFile(ui->tableWidget->item(row,3)->toolTip());
        qDebug()<<url;
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::newConfig()
{
    QString name = QInputDialog::getText(this,tr("Input Name"),tr("Name")).trimmed();
    if(name.isEmpty())
        return ;

    if(m_config->allItems().contains(name)){
        QMessageBox::information(this,tr("Invalid Name"),
                                 tr("Name Existed!"));
        return;
    }

    m_config->setItem(name,currentConfigItem());
    m_config->setDefaultItem(name);
    updateConfigList();
}

void MainWindow::removeConfig()
{
    auto item = ui->listWidget->currentItem();
    if(item)
    {
        m_config->removeItem(item->text());
        updateConfigList();
    }
}

void MainWindow::saveConfig()
{
    auto item = ui->listWidget->currentItem();
    if(item)
    {
        m_config->setItem(item->text(),currentConfigItem());
    }
}

void MainWindow::updateConfigList()
{
    ui->listWidget->clear();
    QStringList names = m_config->allItems();
    ui->listWidget->addItems(names);
    ui->listWidget->setCurrentRow(names.indexOf(m_config->defaultItem()));
}

void MainWindow::updateConfigItem()
{
    auto item = ui->listWidget->currentItem();
    if(item)
    {
        AppConfig::Item configItem;
        m_config->getItem(item->text(),&configItem);
        setConfigItem(configItem);
        m_config->setDefaultItem(item->text());
    }
}

void MainWindow::showFilterDialog()
{
    FilterDialog dlg;
    dlg.setIncludeFilter(m_includeFilter);
    dlg.setExcludeFilter(m_excludeFilter);
    if(dlg.exec()){
        m_includeFilter = dlg.includeFilter();
        m_excludeFilter=dlg.excludeFitler();
    }
}

void MainWindow::changeSyncMode()
{
    SyncMode syncMode = (SyncMode)ui->syncModeCombo->currentData().toInt();
    if(syncMode!=CustomSyncMode){
        m_op = AppConfig::syncOperation(syncMode);
    }else{
        SyncModeConfigDialog dlg;
        dlg.setSyncOperation(m_op);
        if(dlg.exec()){
            m_op = dlg.syncOperation();
        }
    }
}

void MainWindow::setFileInfoToTable(int row, int col, const QFileInfo &file,const QString& rootDir)
{
    auto item1 = XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row,col);
    auto item2 = XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row,col+1);

    QIcon fileIcon;
    QString fileName;
    QString fileSize;
    if(file.exists()){
        fileIcon = QFileIconProvider().icon(file);
        fileName = file.absoluteFilePath().remove(0,rootDir.length()+1);
        if(file.isFile()){
            auto unit = m_unit.calculate(file.size());
            fileSize = QString("%1 %2").arg(QString::number(unit.size,'f',2)).arg(unit.unit);
        }
    }
    item1->setIcon(fileIcon);
    item1->setText(fileName);
    item1->setToolTip(file.absoluteFilePath());
    item2->setText(fileSize);
}

AppConfig::Item MainWindow::currentConfigItem() const
{
    AppConfig::Item item;
    item.srcDir = ui->srcDirEdit->text();
    item.dstDir = ui->dstDirEdit->text();
    item.compareMode = (FileSync::CompareMode)ui->compareModeCombo->currentData().toInt();
    item.op = m_op;
    item.includeFilter = m_includeFilter;
    item.excludeFilter = m_excludeFilter;
    return item;
}

void MainWindow::setConfigItem(const AppConfig::Item &item)
{
    ui->srcDirEdit->setText(item.srcDir);
    ui->dstDirEdit->setText(item.dstDir);
    XCOMBO_BOX_HELPER(ui->compareModeCombo)->setCurrentData(item.compareMode);
    m_op = item.op;
    XCOMBO_BOX_HELPER(ui->syncModeCombo)->setCurrentData(AppConfig::syncMode(m_op));
    m_includeFilter=item.includeFilter;
    m_excludeFilter=item.excludeFilter;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

DirSelector::DirSelector(QAbstractButton *btn, QLineEdit *edit)
    :QObject(btn),m_edit(edit)
{
    Q_ASSERT(btn);
    Q_ASSERT(edit);
    connect(btn,SIGNAL(clicked(bool)),this,SLOT(selectDir()));
    QCompleter* completer = new QCompleter(edit);
    QDirModel* dirModel = new QDirModel(completer);
    dirModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    completer->setModel(dirModel);
    edit->setCompleter(completer);
}

void DirSelector::selectDir()
{
    QString dir = QFileDialog::getExistingDirectory(m_edit,
        tr("Select Dir"),m_edit->text());
    if(!dir.isEmpty())
    {
        m_edit->setText(dir);
    }
}
