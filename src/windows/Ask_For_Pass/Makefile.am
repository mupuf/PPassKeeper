dllibdir = @DLLIBDIR@

if WINDOWS_FOUND
  dllib_LTLIBRARIES = libppk_ask_pass_win32.la
  libppk_ask_pass_win32_la_SOURCES = ask_for_pass_win32.cpp
  libppk_ask_pass_win32_la_LIBADD = -lgdi32
  libppk_ask_pass_win32_la_LDFLAGS = -module -shared -no-undefined -avoid-version
else
endif
