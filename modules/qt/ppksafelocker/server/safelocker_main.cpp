#include <QApplication>
#include "safelock.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  SafeLock safelock;

  app.exec();

  return 0;
} 
