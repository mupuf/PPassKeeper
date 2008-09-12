dllibdir = @DLLIBDIR@

if BUILD_SAVE_TO_FILE
  dllib_LTLIBRARIES = libppk_savetofile_enc.la
  libppk_savetofile_enc_la_SOURCES = ../savetofile.cpp ../savetofile-dir.cpp ../savetofile-mkdir.cpp savetofile_encrypted.cpp base64.h base64.cpp
  libppk_savetofile_enc_la_LDFLAGS = -module -shared -no-undefined -avoid-version
endif
