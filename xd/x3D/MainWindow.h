#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "X3DObject.h"
#include "X3DView.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

protected slots:
	void updatePosition();
	void updateRotation();
	void updateScale();

private:
	Ui::MainWindowClass ui;
	X3DView* m_view;
	X3DObject* m_obj;
};
