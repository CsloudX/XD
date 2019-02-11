#ifndef _X_MDI_WIDGET_H
#define _X_MDI_WIDGET_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStyle>
#include <QCloseEvent>
#include <QApplication>
#include <QSettings>
#include <QFileDialog>
#include <QStatusBar>
#include <QMimeData>
#include <QMessageBox>
#include <QDebug>
#include <QTextDocument>
#include <QRegularExpression>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include "XGlobal.h"

class XMdiFindDialog : public QDialog
{
    Q_OBJECT

signals:
    void find(const QString& text, QTextDocument::FindFlags options);
    void find(const QRegularExpression& regexp, QTextDocument::FindFlags options);

public:
    XMdiFindDialog(QWidget* parent=nullptr):QDialog(parent)
    {
        setWindowFlags(Qt::Tool);
        setWindowTitle(tr("Find"));

        m_labelFindWhat = new QLabel(tr("Find what:"),this);
        m_editText = new QLineEdit(this);
        m_buttonFind = new QPushButton(tr("Find"),this);
        setupButtonShortcut(m_buttonFind,QKeySequence::FindNext);
        connect(m_buttonFind,SIGNAL(clicked(bool)),this,SLOT(findNext()));
        m_buttonFindPrevious=new QPushButton(tr("Find Previous"),this);
        setupButtonShortcut(m_buttonFindPrevious,QKeySequence::FindPrevious);
        connect(m_buttonFindPrevious,SIGNAL(clicked(bool)),this,SLOT(findPrevious()));
        m_buttonCancel=new QPushButton(tr("Cancel"),this);
        connect(m_buttonCancel,SIGNAL(clicked(bool)),this,SLOT(close()));
        m_checkCaseSensitively = new QCheckBox(tr("Case sensitively"),this);
        m_checkMatchWholeWord=new QCheckBox(tr("Match whole word"),this);
        m_checkRegexp=new QCheckBox(tr("Regular Expression"),this);

        m_layout = new QHBoxLayout(this);
        setLayout(m_layout);
        QHBoxLayout* textEditLayout = new QHBoxLayout();
        textEditLayout->addWidget(m_labelFindWhat);
        textEditLayout->addWidget(m_editText);
        QVBoxLayout* leftLayout = new QVBoxLayout();
        leftLayout->addLayout(textEditLayout);
        leftLayout->addStretch();
        leftLayout->addWidget(m_checkCaseSensitively);
        leftLayout->addWidget(m_checkMatchWholeWord);
        leftLayout->addWidget(m_checkRegexp);
        QVBoxLayout* rightLayout =new QVBoxLayout();
        rightLayout->addWidget(m_buttonFind);
        rightLayout->addWidget(m_buttonFindPrevious);
        rightLayout->addStretch();
        rightLayout->addWidget(m_buttonCancel);
        m_layout->addLayout(leftLayout);
        m_layout->addLayout(rightLayout);
    }

    QString text()const{return m_editText->text();}


private:
    void setupButtonShortcut(QPushButton* button, const QKeySequence& shortcut)
    {
        button->setShortcut(shortcut);
    }
    void find(QTextDocument::FindFlags options = QTextDocument::FindFlags())
    {
        QString text = m_editText->text();

        if(m_checkCaseSensitively->isChecked())
            X_SET_BIT(options,QTextDocument::FindCaseSensitively);
        if(m_checkMatchWholeWord->isChecked())
            X_SET_BIT(options,QTextDocument::FindWholeWords);

        if(m_checkRegexp->isChecked())
        {
            emit find(QRegularExpression(text),options);
        }
        else
        {
            emit find(text,options);
        }
    }

public slots:
    void findNext()
    {
        find();
    }
    void findPrevious()
    {
        find(QTextDocument::FindBackward);
    }

private:
    QHBoxLayout*    m_layout;
    QLabel*         m_labelFindWhat;
    QLineEdit*      m_editText;
    QPushButton*    m_buttonFind;
    QPushButton*    m_buttonFindPrevious;
    QPushButton*    m_buttonCancel;
    QCheckBox*      m_checkCaseSensitively;
    QCheckBox*      m_checkMatchWholeWord;
    QCheckBox*      m_checkRegexp;
};

class XMdiChild : public QWidget
{
    Q_OBJECT

signals:
    void copyAvailable(bool available);

public:
    XMdiChild(QWidget* parent=nullptr):QWidget(parent)
    {
        setAttribute(Qt::WA_DeleteOnClose);
    }
    virtual ~XMdiChild(){}

    void newFile()
    {
        static int sequenceNumber = 1;

        m_isUntitled = true;
        m_curFile = tr("document%1.txt").arg(sequenceNumber++);
        setWindowTitle(m_curFile + "[*]");
    }
    bool loadFile(const QString& fileName)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool rs = loadFileEvent(fileName);
        QApplication::restoreOverrideCursor();
        if(rs)
            setCurrentFile(fileName);
        return rs;
    }
    bool save()
    {
        if(m_isUntitled){
            return saveAs();
        }else{
            return saveFile(m_curFile);
        }
    }
    bool saveAs()
    {
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save As"),m_curFile);
        if(fileName.isEmpty())
            return false;

        return saveFile(fileName);
    }
    bool saveFile(const QString& fileName)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool rs = saveFileEvent(fileName);
        QApplication::restoreOverrideCursor();
        if(rs)
            setCurrentFile(fileName);
        return rs;
    }

    const QString& currentFile()const{return m_curFile;}
    QString userFriendlyCurrentFile()const
    {
        return QFileInfo(m_curFile).fileName();
    }

public:
    virtual bool hasSelection()const=0;

protected:
    virtual bool loadFileEvent(const QString& fileName)=0;
    virtual bool saveFileEvent(const QString& fileName)=0;
    virtual void closeEvent(QCloseEvent *event)
    {
        if(maybeSave()){
            event->accept();
        }else{
            event->ignore();
        }
    }

public slots:
    virtual void cut(){}
    virtual void copy(){}
    virtual void paste(){}
    virtual bool find(const QString& text, QTextDocument::FindFlags options)
    {
        Q_UNUSED(text);
        Q_UNUSED(options);
        return false;
    }
    virtual bool find(const QRegularExpression& regexp, QTextDocument::FindFlags options)
    {
        Q_UNUSED(regexp);
        Q_UNUSED(options);
        return false;
    }

private:
    void setCurrentFile(const QString& fileName)
    {
        m_curFile  = QFileInfo(fileName).canonicalFilePath();
        m_isUntitled = false;
        setWindowModified(false);
        setWindowTitle(userFriendlyCurrentFile()+"[*]");
    }
    bool maybeSave()
    {
        if(!isWindowModified())
            return true;

        const QMessageBox::StandardButton ret
                = QMessageBox::warning(this, tr("xNotepad"),
                                       tr("'%1' has been modified.\n"
                                          "Do you want to save your changes?")
                                       .arg(userFriendlyCurrentFile()),
                                       QMessageBox::Save | QMessageBox::Discard
                                       | QMessageBox::Cancel);
        switch (ret) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
        }
        return true;
    }

protected:
    QString m_curFile;
    bool m_isUntitled;
};

class XMdiMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    XMdiMainWindow()
    {
        setUnifiedTitleAndToolBarOnMac(true);
        setAcceptDrops(true);

        m_mdiArea = new QMdiArea(this);
        setCentralWidget(m_mdiArea);

        m_findDialog = new XMdiFindDialog(this);
        connect(m_findDialog,SIGNAL(find(QRegularExpression,QTextDocument::FindFlags)),
            this,SLOT(find(QRegularExpression,QTextDocument::FindFlags)));
        connect(m_findDialog,SIGNAL(find(QString,QTextDocument::FindFlags)),
            this,SLOT(find(QString,QTextDocument::FindFlags)));

        setupActions();
        setupMenus();
        setupToolBars();
        statusBar();

        connect(m_mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateMenus()));
        updateMenus();

        loadSettings();
    }
    virtual ~XMdiMainWindow()
    {
    }

    bool openFile(const QString& fileName)
    {
        auto existing = findMdiChild(fileName);
        if(existing)
        {
            m_mdiArea->setActiveSubWindow(existing);
            return true;
        }

        const bool succeeded = loadFile(fileName);
        if(succeeded)
            statusBar()->showMessage(tr("File loaded"));
        else
            QMessageBox::warning(this,tr("Warning"),tr("File open failed!"));

        return succeeded;
    }

protected slots:
    void newFile()
    {
        auto child = createMdiChild();
        child->newFile();
        child->show();
    }
    void openFile()
    {
        const auto files = QFileDialog::getOpenFileNames(this);
        for(auto& file:files)
        {
            openFile(file);
        }
    }
    void save()
    {
        auto child = activeMdiChild();
        if(child)
        {
            if(child->save())
            {
                statusBar()->showMessage(tr("File saved."));
            }
        }
    }
    void saveAs()
    {
        auto child = activeMdiChild();
        if(child)
        {
            if(child->saveAs())
            {
                statusBar()->showMessage(tr("File saved."));
            }
        }
    }
    void saveAll()
    {
        auto windows = m_mdiArea->subWindowList();
        for(auto& win:windows)
        {
            XMdiChild* child = qobject_cast<XMdiChild*>(win->widget());
            if(!child->save())
                return;
        }
        statusBar()->showMessage(tr("All files saved."));
    }
    void updateRecentFileActions(){}
    void openRencentFile(){}

    void cut()
    {
        auto child = activeMdiChild();
        Q_ASSERT(child);

        if(child)   child->cut();
    }
    void copy()
    {
        auto child = activeMdiChild();
        Q_ASSERT(child);

        if(child)   child->copy();
    }
    void paste()
    {
        auto child = activeMdiChild();
        Q_ASSERT(child);

        if(child)   child->paste();
    }
    void showFindDialog()
    {
        m_findDialog->show();
        m_findDialog->activateWindow();
    }

    void find(const QString& text, QTextDocument::FindFlags options)
    {
        auto child = activeMdiChild();
        Q_ASSERT(child);

        if(!child->find(text,options))
            statusBar()->showMessage(tr("Not found!"),2000);
    }
    void find(const QRegularExpression& regexp, QTextDocument::FindFlags options)
    {
        auto child = activeMdiChild();
        Q_ASSERT(child);

        if(!child->find(regexp,options))
            statusBar()->showMessage(tr("Not found!"),2000);
    }

    void about()
    {
        QMessageBox::about(this,tr("About"),
            tr("By XD(QQ:978028760) *v*"));
    }
    void updateMenus()
    {
        auto child = activeMdiChild();
        bool hasMdiChild = (child!=nullptr);
        m_actSave->setEnabled(hasMdiChild);
        m_actSaveAs->setEnabled(hasMdiChild);
        m_actSaveAll->setEnabled(hasMdiChild);
        m_actPaste->setEnabled(hasMdiChild);
        m_actClose->setEnabled(hasMdiChild);
        m_actCloseAll->setEnabled(hasMdiChild);
        m_actTile->setEnabled(hasMdiChild);
        m_actCascade->setEnabled(hasMdiChild);
        m_actNextWindow->setEnabled(hasMdiChild);
        m_actPreviousWindow->setEnabled(hasMdiChild);
        m_actFind->setEnabled(hasMdiChild);
        m_actFindNext->setEnabled(hasMdiChild);
        m_actFindPrevious->setEnabled(hasMdiChild);

        bool hasSelection = (child && child->hasSelection());
        m_actCut->setEnabled(hasSelection);
        m_actCopy->setEnabled(hasSelection);

        updateMenusEvent(child);
    }
    void updateWindowMenu()
    {
        m_menuWindow->clear();
        m_menuWindow->addAction(m_actTile);
        m_menuWindow->addAction(m_actCascade);
        m_menuWindow->addAction(m_actNextWindow);
        m_menuWindow->addAction(m_actPreviousWindow);

        auto windows = m_mdiArea->subWindowList();
        if(windows.isEmpty())
            return;

        m_menuWindow->addSeparator();
        for(auto& window: windows)
        {
            XMdiChild* child = qobject_cast<XMdiChild*>(window->widget());
            QAction* action = m_menuWindow->addAction(
                child->userFriendlyCurrentFile(),window,[this,window](){
                    m_mdiArea->setActiveSubWindow(window);
                });

            action->setCheckable(true);
            action->setChecked(m_mdiArea->activeSubWindow()==window);
            m_menuWindow->addAction(action);
        }
    }

    XMdiChild* createMdiChild()
    {
        XMdiChild* child = createMdiChildEvent();
		Q_ASSERT(child);
        auto subWindow = m_mdiArea->addSubWindow(child);
        connect(child,SIGNAL(copyAvailable(bool)),m_actCut,SLOT(setEnabled(bool)));
        connect(child,SIGNAL(copyAvailable(bool)),m_actCopy,SLOT(setEnabled(bool)));
        if(m_mdiArea->subWindowList().count()==1)
            subWindow->showMaximized();

        return child;
    }
    void switchLayoutDirection(){}


protected:
    virtual XMdiChild* createMdiChildEvent()=0;
    virtual void updateMenusEvent(XMdiChild* child){Q_UNUSED(child);}

private:
    void setupActions()
    {
        m_actNew  =createAction(tr("&New"),this,SLOT(newFile())
            ,QKeySequence::New,tr("Create a new file."));
        m_actOpen  =createAction(tr("&Open..."),this,SLOT(openFile())
            ,QKeySequence::Open,tr("Open an existing file."));
        m_actSave  =createAction(tr("&Save"),this,SLOT(save())
            ,QKeySequence::Save,tr("Save the document to disk."));
        m_actSaveAs  =createAction(tr("Save &As..."),this,SLOT(saveAs())
            ,QKeySequence::SaveAs,tr("Save the document under a new name."));
        m_actSaveAll  =createAction(tr("Save All"),this,SLOT(saveAll())
            ,0,tr("Save all documents to disk."));
        m_actClose  =createAction(tr("&Close"),m_mdiArea,SLOT(closeActiveSubWindow())
            ,QKeySequence::SaveAs,tr("Close the active window."));
        m_actCloseAll = createAction(tr("Close All"),m_mdiArea,SLOT(closeAllSubWindows())
            ,QKeySequence::SaveAs,tr("Close all the window."));
        m_actExit  =createAction(tr("E&xit"),qApp,SLOT(quit())
            ,QKeySequence::Quit,tr("Exit the application."));

        m_actCut = createAction(tr("Cu&t"),this,SLOT(cut()),QKeySequence::Cut,
            tr("Cut the current selection's contents to the clipboard."));
        m_actCopy = createAction(tr("&Copy"),this,SLOT(copy()),QKeySequence::Copy,
            tr("Copy the current selection's contents to the clipboard."));
        m_actPaste = createAction(tr("&Paste"),this,SLOT(paste()),QKeySequence::Paste,
            tr("Paste the clipboard's contents into the current selection."));
        m_actFind = createAction(tr("&Find..."),this,SLOT(showFindDialog()),QKeySequence::Find,
            tr("Find contents into the current selection."));
        m_actFindNext = createAction(tr("Find &Next"),m_findDialog,SLOT(findNext()),QKeySequence::FindNext,
            tr("Find next contents into the current selection."));
        m_actFindPrevious = createAction(tr("Find &Previous"),m_findDialog,SLOT(findPrevious()),QKeySequence::FindPrevious,
            tr("Find previous contents into the current selection."));

        m_actTile = createAction(tr("&Tile"),m_mdiArea,SLOT(tileSubWindows()),0,
            tr("Tile the windows."));
        m_actCascade = createAction(tr("&Cascade"),m_mdiArea,SLOT(cascadeSubWindows()),0,
            tr("Cascade the windows."));
        m_actNextWindow = createAction(tr("Ne&xt Window"),m_mdiArea,SLOT(activateNextSubWindow()),0,
            tr("Move the focus to the next window."));
        m_actPreviousWindow = createAction(tr("Pre&vious Window"),m_mdiArea,SLOT(activatePreviousSubWindow()),0,
            tr("Move the focus to the previous window."));

        m_actAbout = createAction(tr("&About"),this,SLOT(about()),0,
            tr("Show the application's About box."));
    }
    QAction* createAction(const QString& text,const QObject *receiver, const char *member,
                          const QKeySequence &shortcut = 0,const QString& statusTip=0)
    {
        QAction* action = new QAction(text,this);
        if(shortcut!=0)
            action->setShortcut(shortcut);
        if(statusTip!=0)
            action->setStatusTip(statusTip);
        connect(action,SIGNAL(triggered()),receiver,member);
        return action;
    }
    void setupMenus()
    {
        QMenuBar* menuBar = this->menuBar();
        m_menuFile = menuBar->addMenu(tr("&File"));
        m_menuFile->addAction(m_actNew);
        m_menuFile->addAction(m_actOpen);
        m_menuFile->addAction(m_actSave);
        m_menuFile->addAction(m_actSaveAs);
        m_menuFile->addAction(m_actSaveAll);
        m_menuFile->addSeparator();
        m_menuFile->addAction(m_actClose);
        m_menuFile->addAction(m_actCloseAll);
        m_menuFile->addSeparator();
        m_menuFile->addAction(m_actExit);

        m_menuEdit = menuBar->addMenu(tr("&Edit"));
        m_menuEdit->addAction(m_actCut);
        m_menuEdit->addAction(m_actCopy);
        m_menuEdit->addAction(m_actPaste);
        m_menuEdit->addSeparator();
        m_menuEdit->addAction(m_actFind);
        m_menuEdit->addAction(m_actFindNext);
        m_menuEdit->addAction(m_actFindPrevious);

        m_menuWindow = menuBar->addMenu(tr("&Window"));
        connect(m_menuWindow,SIGNAL(aboutToShow()),this,SLOT(updateWindowMenu()));

        m_menuHelp = menuBar->addMenu(tr("&Help"));
        m_menuHelp->addAction(m_actAbout);
    }
    void setupToolBars()
    {
        auto toolBar = this->addToolBar(tr("toolBar"));
        toolBar->addAction(m_actNew);
        toolBar->addAction(m_actOpen);
        toolBar->addAction(m_actSave);
        toolBar->addSeparator();
        toolBar->addAction(m_actCut);
        toolBar->addAction(m_actCopy);
        toolBar->addAction(m_actPaste);
        toolBar->addSeparator();
        toolBar->addAction(m_actTile);
        toolBar->addAction(m_actCascade);
        toolBar->addAction(m_actNextWindow);
        toolBar->addAction(m_actPreviousWindow);
    }
    bool loadFile(const QString& fileName)
    {
        XMdiChild* child = createMdiChild();
        const bool succeeded = child->loadFile(fileName);
        if(succeeded)
        {
            child->show();
        }
        else
        {
            child->close();
        }
        return succeeded;
    }
    void loadSettings()
    {
        QSettings settings(QApplication::applicationName()+".ini",QSettings::IniFormat);
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    void saveSettings()
    {
        QSettings settings(QApplication::applicationName()+".ini",QSettings::IniFormat);
        settings.setValue("geometry",saveGeometry());
    }
    void prependRecentFile(const QString& fileName);
    bool hasRecentFiles();
    void setRecentFilesVisible(bool visible);
    QMdiSubWindow* findMdiChild(const QString& fileName)
    {
        QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
        for(auto window:m_mdiArea->subWindowList())
        {
            XMdiChild* child = qobject_cast<XMdiChild*>(window->widget());
            if(child->currentFile()==canonicalFilePath)
                return window;
        }
        return nullptr;
    }

protected:
    void closeEvent(QCloseEvent* event)
    {
        m_mdiArea->closeAllSubWindows();
        if(m_mdiArea->currentSubWindow())
        {
            event->ignore();
        }
        else
        {
            saveSettings();
            event->accept();
        }
    }
    void dragEnterEvent(QDragEnterEvent* event)
    {
        event->acceptProposedAction();
    }
    void dropEvent(QDropEvent* event)
    {
        auto urls = event->mimeData()->urls();
        for(auto& url:urls)
        {
            openFile(url.toLocalFile());
        }
    }

protected:
    XMdiChild* activeMdiChild()const
    {
        auto window = m_mdiArea->activeSubWindow();
        if(window)
        {
            return qobject_cast<XMdiChild*>(window->widget());
        }
        return nullptr;
    }

protected:
    enum {MaxRecentFiles = 5};
    QMdiArea* m_mdiArea;

    QAction* m_actNew;
    QAction* m_actOpen;
    QAction* m_actSave;
    QAction* m_actSaveAs;
    QAction* m_actSaveAll;
    QAction* m_actPrint;
    QAction* m_actPrintPreview;
    QAction* m_actClose;
    QAction* m_actCloseAll;
    QAction* m_actExit;

    QAction* m_actCut;
    QAction* m_actCopy;
    QAction* m_actPaste;

    QAction* m_actFind;
    QAction* m_actFindNext;
    QAction* m_actFindPrevious;

    QAction* m_actTile;
    QAction* m_actCascade;
    QAction* m_actNextWindow;
    QAction* m_actPreviousWindow;

    QAction* m_actAbout;

    QMenu* m_menuFile;
    QMenu* m_menuEdit;
    QMenu* m_menuWindow;
    QMenu* m_menuHelp;

    XMdiFindDialog* m_findDialog;
};

#endif //_X_MDI_WIDGET_H
