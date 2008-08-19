dllibdir = @DLLIBDIR@

if BUILD_SAVE_TO_FILE
  dllib_LTLIBRARIES = libppk_savetofile_enc.la
  libppk_savetofile_enc_la_SOURCES = ../savetofile.cpp ../savetofile-dir.cpp ../savetofile-mkdir.cpp savetofile_encrypted.cpp
  libppk_savetofile_enc_la_LDFLAGS = -module -shared
endif
