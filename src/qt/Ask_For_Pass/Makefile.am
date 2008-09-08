dllibdir = @DLLIBDIR@

if QTGUI_FOUND
  dllib_LTLIBRARIES = libppk_ask_pass_qt.la
  libppk_ask_pass_qt_la_SOURCES = ask_for_pass_qt.cpp
  libppk_ask_pass_qt_la_LIBADD = @QTCORE_LIBS@ @QTGUI_LIBS@
  libppk_ask_pass_qt_la_LDFLAGS = -module -shared -no-undefined -avoid-version
  INCLUDES = @QTCORE_CFLAGS@ @QTGUI_CFLAGS@
else
endif
