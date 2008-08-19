#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.62)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AC_CONFIG_SRCDIR([src/tokenizer.h])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

PKG_PROG_PKG_CONFIG

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([mkdir])

AC_PROG_LIBTOOL
LT_LIB_DLLOAD

AC_ARG_ENABLE(ppk-lib, [  --disable-ppk-lib  do not build the ppasskeeper library (enabled)], [arg_ppklib=$enableval])
AC_ARG_ENABLE(gtk,     [  --disable-gtk      do not build the GTK plugin (autodetect)], [arg_gtk=$enableval])
AC_ARG_ENABLE(qt,      [  --disable-qt       do not build the Qt plugin  (autodetect)], [arg_qt=$enableval])
AC_ARG_ENABLE(kde,     [  --disable-kde      do not build the KDE plugin (autodetect)], [arg_kde=$enableval])
AC_ARG_ENABLE(stf,     [  --disable-stf      do not build the SaveToFile plugin (autodetect)], [arg_stf=$enableval])

DLLIBDIR=${libdir}/ppasskeeper
AC_SUBST([DLLIBDIR])
AC_DEFINE_UNQUOTED([DLLIBDIR], ["$DLLIBDIR"], [Path to dl libraries])

AM_CONDITIONAL([BUILD_PPKLIB], [test "x$arg_ppklib" != "xno"])

if test "x$arg_gtk" = "xno"; then
  AM_CONDITIONAL([GTK2_FOUND], [false])
else
  PKG_CHECK_MODULES([GTK2], [gtk+-2.0], [GTK2_FOUND="yes"])
  AM_CONDITIONAL([GTK2_FOUND], [test "x$GTK2_FOUND" = "xyes"])
fi

if test "x$arg_qt" = "xno"; then
  AM_CONDITIONAL([QTCORE_FOUND], [false])
  AM_CONDITIONAL([QTGUI_FOUND], [false])
else
  PKG_CHECK_MODULES([QTCORE], [QtCore], [QTCORE_FOUND="yes"])
  PKG_CHECK_MODULES([QTGUI], [QtGui], [QTGUI_FOUND="yes"])
  AM_CONDITIONAL([QTCORE_FOUND], [test "x$QTCORE_FOUND" = "xyes"])
  AM_CONDITIONAL([QTGUI_FOUND], [test "x$QTGUI_FOUND" = "xyes"])
fi

if test "x$arg_kde" = "xno"; then
  AM_CONDITIONAL([KDE4_FOUND], [false])
else
  AC_CHECK_PROG([KDE4CONFIG], [kde4-config], "yes")
  if test "x$KDE4CONFIG" = "xyes"; then
    AM_CONDITIONAL([KDE4_FOUND], [test "x$KDE4CONFIG" = "xyes"])
  fi
fi

AM_CONDITIONAL([BUILD_SAVE_TO_FILE], [test "x$arg_stf" != "xno"])

AC_OUTPUT(
[Makefile
src/Makefile
src/gtk/Makefile
src/gtk/Ask_For_Pass/Makefile
src/qt/Makefile
src/qt/Ask_For_Pass/Makefile
src/kde/Makefile
src/kde/KWallet/Makefile
src/portable/Makefile
src/portable/SaveToFile/Makefile
src/portable/SaveToFile/encrypted/Makefile
src/portable/SaveToFile/plain_text/Makefile]
)

echo
echo -n "Build the ppasskeeper library : " ; if test "x$arg_ppklib" = "xno"; then echo "no"; else echo "yes"; fi
echo
echo "Enabled optional modules :"
echo "-------------------------------------------"
echo -n "gtk : " ; test -z "$GTK2_FOUND" && echo "no" || echo "$GTK2_FOUND"
echo -n "qt  : " ; test -z "$QTGUI_FOUND" && echo "no" || echo "$QTGUI_FOUND"
echo -n "kde : " ; test -z "$KDE4CONFIG" && echo "no" || echo "$KDE4CONFIG"
echo -n "save_to_file : " ; if test "x$arg_stf" = "xno"; then echo "no"; else echo "yes"; fi
