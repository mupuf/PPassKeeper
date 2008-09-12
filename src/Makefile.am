SUBDIRS = gtk qt kde portable

if BUILD_PPKLIB
  lib_LTLIBRARIES = libppasskeeper.la

  libppasskeeper_la_SOURCES = ppasskeeper.cpp ppasskeeper-module.h ppasskeeper.h settings.h tokenizer.h ppk_modules.h ppk_modules.cpp
  libppasskeeper_la_LIBADD = @LIBADD_DLOPEN@
  libppasskeeper_la_LDFLAGS = -no-undefined -version-info 1:0:0

  library_includedir=$(includedir)
  library_include_HEADERS = ppasskeeper.h

  pkgconfigdir = $(libdir)/pkgconfig
  pkgconfig_DATA = ppasskeeper.pc

  AM_CPPFLAGS = -DDIRECTORY_PATH=\"@DLLIBDIR@\" @DEBUG_CPPFLAG@
endif

