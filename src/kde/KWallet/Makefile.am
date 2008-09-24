dllibdir = @DLLIBDIR@

if KDE4_FOUND
  dllib_LTLIBRARIES = libppk_kwallet.la
  libppk_kwallet_la_SOURCES = kwallet.cpp list_pwd.cpp
  libppk_kwallet_la_LIBADD = -lkdeinit4_kwalletd
  libppk_kwallet_la_LDFLAGS = -module -shared -no-undefined -L`kde4-config --prefix`/lib -avoid-version
  INCLUDES = @KDE4_INCLUDES@
else
endif
