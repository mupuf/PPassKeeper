bin_PROGRAMS = qtpasskeeper$(EXEEXT)
MOC_SRC = mainwindow_moc.cpp
CLEANFILES = $(MOC_SRC)
qtpasskeeper_SOURCES = main.cpp \
		       mainwindow.h mainwindow.cpp $(MOC_SRC)
qtpasskeeper_LDADD = -L../../src -lppasskeeper @QTCORE_LIBS@ @QTGUI_LIBS@
qtpasskeeper_DEPENDENCIES = ../../src/libppasskeeper.la

EXTRA_DIST = qtpasskeeper.desktop
Applicationsdir = $(datadir)/applications
Applications_DATA = qtpasskeeper.desktop

%_moc.cpp: %.h
	@MOC@ -o $@ $<

INCLUDES = -I../../src @QTCORE_CFLAGS@ @QTGUI_CFLAGS@
