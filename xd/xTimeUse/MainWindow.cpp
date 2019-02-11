#include "MainWindow.h"
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.lineEdit_PathFile, SIGNAL(editingFinished()), this, SLOT(parse()));

	ui.lineEdit_PathFile->setText("test.txt");
	parse();
}

void MainWindow::parse()
{
	QString fileName = ui.lineEdit_PathFile->text();
	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("Warning"), file.errorString());
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
