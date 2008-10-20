bin_PROGRAMS = qtpasskeeper$(EXEEXT)
MOC_SRC = mainwindow_moc.cpp passwordlistmodel_moc.cpp
UI_INC = mainwindow_ui.h
CLEANFILES = $(MOC_SRC) $(UI_INC)
qtpasskeeper_SOURCES = main.cpp \
		       mainwindow.h mainwindow.cpp \
		       passwordlistmodel.h passwordlistmodel.cpp \
		       $(MOC_SRC) $(UI_INC)
qtpasskeeper_LDADD = -L../../src -lppasskeeper @QTCORE_LIBS@ @QTGUI_LIBS@
qtpasskeeper_DEPENDENCIES = $(MOC_SRC) $(UI_INC) ../../src/libppasskeeper.la

EXTRA_DIST = qtpasskeeper.desktop
Applicationsdir = $(datadir)/applications
Applications_DATA = qtpasskeeper.desktop

%_moc.cpp: %.h
	@MOC@ -o $@ $<

%_ui.h: %.ui
	@UIC@ -o $@ $<

INCLUDES = -I../../src @QTCORE_CFLAGS@ @QTGUI_CFLAGS@
