#include "MainWindow.h"
#include "X3DView.h"
#include <qt/XSpinBoxHelper.h>
#include <Qt/XPauseDialog.h>
#include <QMatrix>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QMatrix mt;
	qHash(mt);

	m_obj = new X3DCube();

	m_view = new X3DView(this);
	m_view->addObject(m_obj);

	ui.lLayout->addWidget(m_view);

	X_SPINBOX_HELPER(ui.doubleSpinBoxPX)->setStepWidget(ui.labelPX);
	X_SPINBOX_HELPER(ui.doubleSpinBoxPY)->setStepWidget(ui.labelPY);
	X_SPINBOX_HELPER(ui.doubleSpinBoxPZ)->setStepWidget(ui.labelPZ);
	connect(ui.doubleSpinBoxPX, SIGNAL(valueChanged(double)), this, SLOT(updatePosition()));
	connect(ui.doubleSpinBoxPY, SIGNAL(valueChanged(double)), this, SLOT(updatePosition()));
	connect(ui.doubleSpinBoxPZ, SIGNAL(valueChanged(double)), this, SLOT(updatePosition()));


	X_SPINBOX_HELPER(ui.doubleSpinBoxRX)->setStepWidget(ui.labelRX);
	X_SPINBOX_HELPER(ui.doubleSpinBoxRY)->setStepWidget(ui.labelRY);
	X_SPINBOX_HELPER(ui.doubleSpinBoxRZ)->setStepWidget(ui.labelRZ);
	connect(ui.doubleSpinBoxRX, SIGNAL(valueChanged(double)), this, SLOT(updateRotation()));
	connect(ui.doubleSpinBoxRY, SIGNAL(valueChanged(double)), this, SLOT(updateRotation()));
	connect(ui.doubleSpinBoxRZ, SIGNAL(valueChanged(double)), this, SLOT(updateRotation()));


	X_SPINBOX_HELPER(ui.doubleSpinBoxSX)->setStepWidget(ui.labelSX);
	X_SPINBOX_HELPER(ui.doubleSpinBoxSY)->setStepWidget(ui.labelSY);
	X_SPINBOX_HELPER(ui.doubleSpinBoxSZ)->setStepWidget(ui.labelSZ);
	connect(ui.doubleSpinBoxSX, SIGNAL(valueChanged(double)), this, SLOT(updateScale()));
	connect(ui.doubleSpinBoxSY, SIGNAL(valueChanged(double)), this, SLOT(updateScale()));
	connect(ui.doubleSpinBoxSZ, SIGNAL(valueChanged(double)), this, SLOT(updateScale()));
}

void MainWindow::updatePosition()
{
	m_obj->setPosition(ui.doubleSpinBoxPX->value(), ui.doubleSpinBoxPY->value(), ui.doubleSpinBoxPZ->value());
	m_view->update();
}

void MainWindow::updateRotation()
{
	m_obj->setRotation(ui.doubleSpinBoxRX->value(), ui.doubleSpinBoxRY->value(), ui.doubleSpinBoxRZ->value());
	m_view->update();
}

void MainWindow::updateScale()
{
	m_obj->setScale(ui.doubleSpinBoxSX->value(), ui.doubleSpinBoxSY->value(), ui.doubleSpinBoxSZ->value());
	m_view->update();
}
