dllibdir = @DLLIBDIR@

if KDE4_FOUND
  dllib_LTLIBRARIES = libppk_kwallet.la
  libppk_kwallet_la_SOURCES = kwallet.cpp
  libppk_kwallet_la_LIBADD = -lkdeui -lkdeinit4_kwalletd
  libppk_kwallet_la_LDFLAGS = -module -shared -no-undefined -L`kde4-config --prefix`/lib -avoid-version
  INCLUDES = -I`kde4-config --prefix`/include
else
endif
