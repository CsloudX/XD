#ifndef _X_MAINWINDOW_H
#define _X_MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

class XMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit XMainWindow(QWidget *parent = nullptr)
        :QMainWindow(parent),m_isUntitled(true)
    {
        setUnifiedTitleAndToolBarOnMac(true);
        setAcceptDrops(true);

        setupActions();
        setupMenus();
        setupToolBars();
        createStatusBar();

        setCurrentFile(QString());
        loadSettings();
    }
    virtual ~XMainWindow(){}

    bool loadFile(const QString& fileName)
    {
		QString err;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool ret = loadFileEvent(fileName,&err);
        QApplication::restoreOverrideCursor();

		if (ret)
			setCurrentFile(fileName);
		else
			QMessageBox::warning(this, tr("Warning"), err);

        statusBar()->showMessage(ret?tr("File loaded."):tr("File load failed."),2000);
        return ret;
    }

protected:
    virtual void newFileEvent()=0;
    virtual bool loadFileEvent(const QString& fileName,QString* err)=0;
    virtual bool saveFileEvent(const QString& fileName,QString* err)=0;
	virtual void printEvent(QPrinter *printer) {}
    virtual void cutEvent(){}
    virtual void copyEvent(){}
    virtual void pasteEvent(){}
	virtual void removeEvent(){}

private slots:
    void newFile()
    {
        if(maybeSave()){
            newFileEvent();
            setCurrentFile(QString());
        }
    }
    void open()
    {
        if(maybeSave()){
            QString fileName = QFileDialog::getOpenFileName(this);
            if(!fileName.isEmpty()){
                loadFile(fileName);
            }
        }
    }
    bool save()
    {
        if(m_curFile.isEmpty())
            return saveAs();
        else
            return saveFile(m_curFile);
    }
    bool saveAs()
    {
        QFileDialog dialog(this);
        dialog.setWindowModality(Qt::WindowModal);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        if (dialog.exec() != QDialog::Accepted)
            return false;
        return saveFile(dialog.selectedFiles().first());
    }
	void print()
	{
		QPrinter printer;
		QPrintDialog dlg(&printer, this);
		if (dlg.exec() == QDialog::Accepted)
		{
			print(&printer);
		}
	}
	void printPreview()
	{
		QPrinter printer;
		QPrintPreviewDialog preview(&printer, this);
		connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(print(QPrinter *)));
		preview.exec();
	}
	void print(QPrinter *printer) { printEvent(printer); }
    void cut(){cutEvent();}
    void copy(){copyEvent();}
    void paste(){pasteEvent();}
	void remove() 
	{
		if (QMessageBox::question(this, tr("Delete Items"), tr("Are you sure delete?")) != QMessageBox::Yes)
			return;

		removeEvent(); 
	}
    void about()
    {
        QMessageBox::about(this, tr("About"),
            "<b>"+m_windowTitle+"</b><br>"+m_aboutText );
    }

protected:
    void setWindowTitle(const QString& title)
    {
        m_windowTitle=title;
        setCurrentFile(m_curFile);
    }
    void setAboutText(const QString& text)
    {
        m_aboutText=text;
    }
	void loadSettings()
	{
		QSettings settings(QCoreApplication::applicationName(), QSettings::IniFormat);
		restoreGeometry(settings.value("geometry").toByteArray());
		restoreState(settings.value("state").toByteArray());
	}
	void saveSettings()
	{
		QSettings settings(QCoreApplication::applicationName(), QSettings::IniFormat);
		settings.setValue("geometry", saveGeometry());
		settings.setValue("state", saveState());
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
    void closeEvent(QCloseEvent* event)
    {
        if(maybeSave()){
            saveSettings();
            event->accept();
        }else{
            event->ignore();
        }
    }

private:
    bool saveFile(const QString& fileName)
    {
		QString err;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool ret = saveFileEvent(fileName,&err);
        QApplication::restoreOverrideCursor();

		if (ret)
			setCurrentFile(fileName);
		else
			QMessageBox::warning(this, tr("Warning"), err);
        statusBar()->showMessage(ret?tr("File saved."):tr("File save failed."), 2000);

        return ret;
    }
    bool maybeSave()
    {
        if (!isWindowModified())
            return true;

        const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
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
    void setCurrentFile(const QString& fileName)
    {
        m_curFile = fileName;
        setWindowModified(false);
        QMainWindow::setWindowTitle(m_curFile+"[*] - "+m_windowTitle);
    }

    void setupActions()
    {
        m_actNew = createAction(tr("&New"),this,SLOT(newFile()),QKeySequence::New,
            tr("Create a new file."));
        m_actOpen= createAction(tr("&Open"),this,SLOT(open()),QKeySequence::Open,
            tr("Open an existing file."));
        m_actSave = createAction(tr("&Save"),this,SLOT(save()),QKeySequence::Save,
            tr("Save the document to disk."));
        m_actSaveAs = createAction(tr("Save &As"),this,SLOT(saveAs()),QKeySequence::SaveAs,
            tr("Save the document under a new name."));
		m_actPrint = createAction(tr("&Print..."), this, SLOT(print()), QKeySequence::Print,
			tr("Print the document."));
		m_actPrintPreview = createAction(tr("Print Preview..."), this, SLOT(printPreview()));
        m_actExit = createAction(tr("E&xit"),qApp,SLOT(closeAllWindows()),QKeySequence::Quit,
            tr("Exit the application."));

        m_actCut = createAction(tr("Cu&t"),this,SLOT(cut()),QKeySequence::Cut,
            tr("Cut the current selection's contents to the clipboard."));
        m_actCopy = createAction(tr("&Copy"),this,SLOT(copy()),QKeySequence::Copy,
            tr("Copy the current selection's contents to the clipboard."));
        m_actPaste = createAction(tr("&Paste"),this,SLOT(paste()),QKeySequence::Paste,
            tr("Paste the clipboard's contents into the current selection."));
		m_actDelete = createAction(tr("&Delete"), this, SLOT(remove()), QKeySequence::Delete,
			tr("Delete the current selection's contents."));
        m_actCut->setEnabled(false);
        m_actCopy->setEnabled(false);

        m_actAbout = createAction(tr("&About"),this,SLOT(about()),0,
            tr("Show the application's About box."));
    }
    void setupMenus()
    {
        m_menuFile=menuBar()->addMenu(tr("&File"));
        m_menuFile->addAction(m_actNew);
        m_menuFile->addAction(m_actOpen);
        m_menuFile->addAction(m_actSave);
        m_menuFile->addAction(m_actSaveAs);
        m_menuFile->addSeparator();
		m_menuFile->addAction(m_actPrint);
		m_menuFile->addAction(m_actPrintPreview);
		m_menuFile->addSeparator();
        m_menuRecent = m_menuFile->addMenu(tr("Recent"));
        connect(m_menuRecent,SIGNAL(aboutToShow()),this,SLOT(updateRecentActions()));
        m_recentSeparator=m_menuFile->addSeparator();
        m_menuFile->addAction(m_actExit);

        m_menuEdit = menuBar()->addMenu(tr("&Edit"));
        m_menuEdit->addAction(m_actCut);
        m_menuEdit->addAction(m_actCopy);
        m_menuEdit->addAction(m_actPaste);
		m_menuEdit->addAction(m_actDelete);

        m_menuHelp = menuBar()->addMenu(tr("&Help"));
        m_menuHelp->addAction(m_actAbout);
    }
    void setupToolBars()
    {
        m_toolBar = addToolBar(QString());
		m_toolBar->setObjectName("ToolBar");
        m_toolBar->addAction(m_actNew);
        m_toolBar->addAction(m_actOpen);
        m_toolBar->addAction(m_actSave);
		m_toolBar->addAction(m_actPrint);
        m_toolBar->addSeparator();
        m_toolBar->addAction(m_actCut);
        m_toolBar->addAction(m_actCopy);
        m_toolBar->addAction(m_actPaste);
		m_toolBar->addAction(m_actDelete);
    }
    void createStatusBar()
    {
        statusBar()->showMessage(tr("Ready"));
    }

protected:
    QAction* m_actNew;
    QAction* m_actOpen;
    QAction* m_actSave;
    QAction* m_actSaveAs;
	QAction* m_actPrint;
	QAction* m_actPrintPreview;
    QAction* m_actExit;

    QAction* m_actCut;
    QAction* m_actCopy;
    QAction* m_actPaste;
	QAction* m_actDelete;

    QAction* m_actAbout;

    QMenu*  m_menuFile;
    QMenu*  m_menuEdit;
    QMenu*  m_menuHelp;

    QMenu*  m_menuRecent;
    QAction* m_recentSeparator;

    QToolBar* m_toolBar;

private:
    QString m_windowTitle;
    QString m_aboutText;
    QString m_curFile;
    bool    m_isUntitled;
};

#endif // _X_MAINWINDOW_H
