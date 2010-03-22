#include <QtGui/QApplication>
#include "mainwindow.h"

#include <stdio.h>
#include "../safelock.h"
#include "../crypt.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	SafeLock sl("poulpage.locked");
	sl.open("passphrase");
	sl.add(SFEntry("s\"poulpissimerie\":\"pompeuse\"\n"));
	sl.close();

	return 0;
}
