# -------------------------------------------------
# Project created by QtCreator 2010-03-21T15:23:28
# -------------------------------------------------
QT += network
TARGET = project
CONFIG += link_pkgconfig
PKGCONFIG = ppasskeeper
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    ../entry.cpp \
    ../safelock.cpp
HEADERS += mainwindow.h \
    ../entry.h \
    ../safelock.h
FORMS += mainwindow.ui
