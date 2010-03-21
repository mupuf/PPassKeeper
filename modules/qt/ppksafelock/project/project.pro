# -------------------------------------------------
# Project created by QtCreator 2010-03-21T15:23:28
# -------------------------------------------------
QT += network
TARGET = project
CONFIG += link_pkgconfig
PKGCONFIG = ppasskeeper openssl
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    ../entry.cpp \
    ../safelock.cpp \
    ../crypt.cpp \
    ../sha512.c
HEADERS += mainwindow.h \
    ../entry.h \
    ../safelock.h \
    ../sha512.h
FORMS += mainwindow.ui
