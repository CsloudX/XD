#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include "MainWindow.h"

#ifdef _DEBUG
#include <vld.h>
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    MainWindow w;
	w.show();
	return a.exec();
}
