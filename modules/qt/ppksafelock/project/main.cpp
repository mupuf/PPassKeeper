#include <QtGui/QApplication>
#include "mainwindow.h"

#include "../entry.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Entry e("s\"mupuf\":\"jarjargeek\"");

    return a.exec();
}
