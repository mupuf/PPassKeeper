dllibdir = @DLLIBDIR@

if BUILD_SAVE_TO_FILE
  dllib_LTLIBRARIES = libppk_savetofile_pt.la
  libppk_savetofile_pt_la_SOURCES = ../savetofile.cpp ../savetofile-dir.cpp ../savetofile-mkdir.cpp ../list_pwd.h ../list_pwd.cpp savetofile_plain_text.cpp
  libppk_savetofile_pt_la_LDFLAGS = -module -shared -no-undefined -avoid-version

  AM_CPPFLAGS = @HAVE_CHMOD@
endif

