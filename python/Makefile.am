if BUILD_PYTHON
  pyexec_LTLIBRARIES = ppk.la

  ppk_la_SOURCES = ppk_python.c
  ppk_la_LIBADD = -L../src -lppasskeeper @PYTHON_LIBS@
  ppk_la_LDFLAGS = -module -shared -no-undefined -avoid-version
  ppk_la_DEPENDENCIES = ../src/libppasskeeper.la

  INCLUDES = @PYTHON_INCS@
endif
