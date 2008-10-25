bin_PROGRAMS = ppasskeeper-qt$(EXEEXT)
MOC_SRC = mainwindow_moc.cpp passwordlistmodel_moc.cpp
UI_INC = mainwindow_ui.h
ppasskeeper_qt_SOURCES = $(MOC_SRC) $(UI_INC) main.cpp \
		       mainwindow.h mainwindow.cpp \
		       passwordlistmodel.h passwordlistmodel.cpp
BUILT_SOURCES = $(MOC_SRC) $(UI_INC)
CLEANFILES = $(BUILT_SOURCES)
ppasskeeper_qt_LDADD = -L../../src -lppasskeeper @QTCORE_LIBS@ @QTGUI_LIBS@
ppasskeeper_qt_DEPENDENCIES = ../../src/libppasskeeper.la

EXTRA_DIST = ppasskeeper-qt.desktop
Applicationsdir = $(datadir)/applications
Applications_DATA = ppasskeeper-qt.desktop

%_moc.cpp: %.h
	@MOC@ -o $@ $<

%_ui.h: %.ui
	@UIC@ -o $@ $<

INCLUDES = -I../../src @QTCORE_CFLAGS@ @QTGUI_CFLAGS@
