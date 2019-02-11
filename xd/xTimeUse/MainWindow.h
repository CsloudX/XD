#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "XTimeUseNode.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

private slots:
	void parse();

private:
	void setNode(QTreeWidgetItem* item, XTimeUseNode* node);

private:
	Ui::MainWindowClass ui;
};
