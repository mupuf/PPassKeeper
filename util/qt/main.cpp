#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("MuPuF");
	app.setApplicationName(QObject::tr("PPassKeeper Manager : Manage your passwords easily"));

	MainWindow *mw = new MainWindow;
	mw->show();

	return app.exec();
}
