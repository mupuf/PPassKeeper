if BUILD_UTIL
  bin_PROGRAMS = ppasskeeper$(EXEEXT)

  ppasskeeper_SOURCES = ppasskeeper-util.c
  ppasskeeper_LDADD = -L../src -lppasskeeper
  ppasskeeper_DEPENDENCIES = ../src/libppasskeeper.la

  INCLUDES = -I../src

  man1_MANS = ppasskeeper.1
endif
