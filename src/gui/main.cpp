#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

#ifdef NDEBUG
	QDir::setCurrent(QApplication::applicationDirPath());
#endif
	a.setStyle(QStyleFactory::create("Fusion"));

	MainWindow w;
	w.show();

	return a.exec();
}
