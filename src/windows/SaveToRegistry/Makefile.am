dllibdir = @DLLIBDIR@

if WINDOWS_FOUND
  dllib_LTLIBRARIES = libppk_savetoregistry.la
  libppk_savetoregistry_la_SOURCES = savetoregistry.cpp list_pwd.cpp
  libppk_savetoregistry_la_LDFLAGS = -module -shared -no-undefined -avoid-version
else
endif
