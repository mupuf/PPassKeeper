#include <QtGui/QApplication>
#include "mainwindow.h"

#include <stdio.h>
#include "../safelock.h"
#include "../crypt.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	int res=cryptToFile("chacal", "gaga", "je suis chacal !");

	char* data;
	res=decryptFromFile("gaga",	&data, "je suis un chacal !");
	printf("res=%i\n", res);

	//Entry e("s\"mupuf\":\"jarjargeek\"\n");
	/*SafeLock sl("poulpage.locked");

	sl.open("passphrase");

	sl.add(Entry("s\"chacaloterie\":\"pompeuse\"\n"));

	sl.close();*/

	return 0;
}
