#include "MainWindow.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDropEvent>
#include <QMimeData>
#include <QCompleter>
#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setAcceptDrops(true);

	QCompleter* completer = new QCompleter(this);
	QFileSystemModel* model = new QFileSystemModel(completer);
	model->setRootPath("");
	completer->setModel(model);
	ui.lineEdit_PathFile->setCompleter(completer);

	connect(ui.toolButton, SIGNAL(clicked()), this, SLOT(selectFile()));
	connect(ui.lineEdit_PathFile, SIGNAL(editingFinished()), this, SLOT(parse()));
	ui.treeWidget->setColumnWidth(0, width() / 2);
}


void MainWindow::selectFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Log"),
		ui.lineEdit_PathFile->text(), tr("Text files (*.txt *.log);;All files (*.*)"));
	if (!fileName.isEmpty())
	{
		ui.lineEdit_PathFile->setText(fileName);
		parse();
	}
}

void MainWindow::parse()
{
	QString fileName = ui.lineEdit_PathFile->text();
	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		statusBar()->showMessage(file.errorString());
		return;
	}

	QStringList lines;
	while (!file.atEnd()) {
		lines.append(file.readLine());
	}

	int lineNum = 0;
	QList<XTimeUseNode*> nodes;
	while (true) {
		XTimeUseNode* node=new XTimeUseNode();
		if (node->parse(lines, &lineNum)) {
			nodes.append(node);
		}
		else {
			delete node;
			break;
		}
	}

	ui.treeWidget->clear();
	for (XTimeUseNode* node : nodes) {
		QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
		ui.treeWidget->addTopLevelItem(item);
		setNode(item, node);
	}

	qDeleteAll(nodes);
}

void MainWindow::setNode(QTreeWidgetItem* item, XTimeUseNode* node)
{
	item->setText(0, node->text());
	item->setText(1, QString("%1ms").arg(node->timeUse()));
	if (node->parent()) {
		item->setText(2, QString("%1%").arg(node->timeUse() * 100 / node->parent()->timeUse()));
	}

	for (XTimeUseNode* child : node->children()) {
		QTreeWidgetItem* childItem = new QTreeWidgetItem(item);
		setNode(childItem, child);
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	ui.lineEdit_PathFile->setText(urls.first().toLocalFile());
	parse();
}
