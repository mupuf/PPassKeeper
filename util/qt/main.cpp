#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName(QString::fromUtf8("MuPuF.org"));
	app.setApplicationName(QObject::tr("PPassKeeper Manager : Manage your passwords easily"));

	MainWindow *mw = new MainWindow;
	mw->show();

	return app.exec();
}
