dllibdir = @DLLIBDIR@

if GTK2_FOUND
  dllib_LTLIBRARIES = libppk_ask_pass_gtk.la
  libppk_ask_pass_gtk_la_SOURCES = ask_for_pass_gtk.cpp
  libppk_ask_pass_gtk_la_LIBADD = @GTK2_LIBS@
  libppk_ask_pass_gtk_la_LDFLAGS = -module -shared
  INCLUDES = @GTK2_CFLAGS@
else
endif
