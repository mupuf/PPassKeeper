#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.62)
AC_INIT(PPassKeeper, 1.0, http://ppasskeeper.mupuf.org)
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

LT_INIT([win32-dll])
LT_OUTPUT
LT_LIB_DLLOAD

AC_ARG_ENABLE(debug,   [  --enable-debug     enable debug output (disabled)], [arg_debug=$enableval])
AC_ARG_ENABLE(ppk-lib, [  --disable-ppk-lib  do not build the ppasskeeper library (enabled)], [arg_ppklib=$enableval])
AC_ARG_ENABLE(util,    [  --disable-util     do not build the command-line utility (enabled)], [arg_util=$enableval])
AC_ARG_ENABLE(gui-util,[  --disable-gui-util do not build the graphical utility (enabled)], [arg_gui_util=$enableval])
AC_ARG_ENABLE(plugins, [  --disable-plugins  do not build any plugins], [arg_plugins=$enableval])
AC_ARG_ENABLE(gtk,     [  --disable-gtk      do not build the GTK plugin (autodetect)], [arg_gtk=$enableval])
AC_ARG_ENABLE(qt,      [  --disable-qt       do not build the Qt plugin  (autodetect)], [arg_qt=$enableval])
AC_ARG_ENABLE(kde,     [  --disable-kde      do not build the KDE plugin (autodetect)], [arg_kde=$enableval])
AC_ARG_ENABLE(gnome,   [  --disable-gnome    do not build the GNOME plugin (autodetect)], [arg_gnome=$enableval])
AC_ARG_ENABLE(webdav,  [  --disable-webdav   do not build the WebDAV plugin (autodetect)], [arg_webdav=$enableval])
AC_ARG_ENABLE(win32,   [  --disable-win32    do not build the WIN32 plugins (autodetect)], [arg_win32=$enableval])
AC_ARG_ENABLE(stf,     [  --disable-stf      do not build the SaveToFile plugin (autodetect)], [arg_stf=$enableval])
AC_ARG_ENABLE(python,  [  --disable-python   do not build the Python binding (autodetect)], [arg_python=$enableval])

PKG_CHECK_MODULES([ELEKTRA], [elektra])

case "$host" in
  *mingw*)
    HAVE_CHMOD=
	
	if test "x$arg_win32" != "xno"; then
		AM_CONDITIONAL([WINDOWS_FOUND], [true])
		WINDOWS_FOUND="yes"
	else
		AM_CONDITIONAL([WINDOWS_FOUND], [false])
		WINDOWS_FOUND="no"	
	fi
	;;
  *)
    HAVE_CHMOD="-DHAVE_CHMOD"
	AM_CONDITIONAL([WINDOWS_FOUND], [false])
	WINDOWS_FOUND="no"
    ;;
esac
AC_SUBST([HAVE_CHMOD])


if test "x$arg_debug" = "xyes"; then
  DEBUG_CPPFLAG='-DDEBUG_MSG'
else
  DEBUG_CPPFLAG=
fi
AC_SUBST([DEBUG_CPPFLAG])

DLLIBDIR=${libdir}/ppasskeeper
AC_SUBST([DLLIBDIR])
AC_DEFINE_UNQUOTED([DLLIBDIR], ["$DLLIBDIR"], [Path to dl libraries])

if test "x$arg_plugins" = "xno"; then
  arg_gtk=no
  arg_qt=no
  arg_kde=no
  arg_gnome=no
  arg_webdav=no
  arg_stf=no
fi

if test "x$arg_gtk" = "xno"; then
  AM_CONDITIONAL([GTK2_FOUND], [false])
else
  PKG_CHECK_MODULES([GTK2], [gtk+-2.0], [GTK2_FOUND="yes"], [echo "$GTK2_PKG_ERRORS"])
  AM_CONDITIONAL([GTK2_FOUND], [test "x$GTK2_FOUND" = "xyes"])
fi

if test "x$arg_qt" != "xno" -o "x$arg_gui_util" != "xno"; then
dnl 	we want to build the Qt plugin OR the GUI utility
dnl	do Qt checks
  PKG_CHECK_MODULES([QTCORE], [QtCore], [QTCORE_FOUND="yes"], [echo "$QTCORE_PKG_ERRORS"])
  PKG_CHECK_MODULES([QTGUI], [QtGui], [QTGUI_FOUND="yes"], [echo "$QTGUI_PKG_ERRORS"])
  MOC=`$PKG_CONFIG --variable=moc_location QtCore`
  UIC=`$PKG_CONFIG --variable=uic_location QtCore`
  AC_SUBST([MOC])
  AC_SUBST([UIC])
fi

if test "x$arg_qt" = "xno"; then
  BUILD_QT="no"
  AM_CONDITIONAL([BUILD_QT], [false])
else
  test "x$QTGUI_FOUND" = "xyes" && BUILD_QT="yes" || BUILD_QT="no"
  AM_CONDITIONAL([BUILD_QT], [test "x$QTGUI_FOUND" = "xyes"])
fi

if test "x$arg_gui_util" = "xno"; then
  BUILD_QT_GUI="no"
  AM_CONDITIONAL([BUILD_QT_GUI], [false])
else
  test "x$QTGUI_FOUND" = "xyes" && BUILD_QT_GUI="yes" || BUILD_QT_GUI="no"
  AM_CONDITIONAL([BUILD_QT_GUI], [test "x$QTGUI_FOUND" = "xyes"])
fi

if test "x$arg_kde" = "xno"; then
  AM_CONDITIONAL([KDE4_FOUND], [false])
else
  AC_CHECK_PROG([KDE4CONFIG], [kde4-config], "yes")

  if test "x$KDE4CONFIG" = "xyes"; then
    KDE4_INCLUDES=
    for p in "`kde4-config --prefix`/include" \
      "`kde4-config --prefix`/include/KDE" \
      "`kde4-config --prefix`/include/kde4"
    do
      AC_MSG_CHECKING([for kwallet.h in $p])
      if test -f "${p}/kwallet.h"; then
        KDE4_INCLUDES="-I${p}"
	AC_MSG_RESULT([yes])
        break
      fi
      AC_MSG_RESULT([no])
    done
    AM_CONDITIONAL([KDE4_FOUND], [test ! -z "$KDE4_INCLUDES"])
    AC_SUBST([KDE4_INCLUDES])
  else
    AM_CONDITIONAL([KDE4_FOUND], [false])
  fi
fi

AM_CONDITIONAL([BUILD_SAVE_TO_FILE], [test "x$arg_stf" != "xno"])

if test "x$arg_python" = "xno"; then
  PYTHON_FOUND="no"
  AM_CONDITIONAL([BUILD_PYTHON], [false])
else
  AM_PATH_PYTHON(, [PYTHON_FOUND="yes"], [PYTHON_FOUND="no"])

  AC_MSG_CHECKING([for ${PYTHON}-config])
  if test ! -x "${PYTHON}-config"; then
	  AC_MSG_RESULT([no])
	  PYTHON_FOUND="no"
  else
	  AC_MSG_RESULT([yes])
	  PYTHON_LIBS=`${PYTHON}-config --libs`
	  PYTHON_INCS=`${PYTHON}-config --includes`
	  AC_SUBST([PYTHON_LIBS])
	  AC_SUBST([PYTHON_INCS])
  fi

  AM_CONDITIONAL([BUILD_PYTHON], [test "x${PYTHON_FOUND}" = "xyes"])
fi

AM_CONDITIONAL([BUILD_UTIL], [test "x$arg_util" != "xno"])



if test "x$arg_gnome" = "xno"; then
  AM_CONDITIONAL([GKEYRING_FOUND], [false])
else
  GTK_REQUIRED=2.6.0
  LIBGLADE_REQUIRED=2.0.0
  LIBGNOMEUI_REQUIRED=2.6.0
  GNOME_KEYRING_REQUIRED=0.3.2

  PKG_CHECK_MODULES([GNOME_KEYRING],
                  [gtk+-2.0 >= $GTK_REQUIRED             		
                  libglade-2.0 >= $LIBGLADE_REQUIRED       	
                  libgnomeui-2.0 >= $LIBGNOMEUI_REQUIRED
		  gnome-keyring-1 >= $GNOME_KEYRING_REQUIRED],
		  [GKEYRING_FOUND="yes"], [echo "$GNOME_KEYRING_PKG_ERRORS"])
  AM_CONDITIONAL([GKEYRING_FOUND], [test "x$GKEYRING_FOUND" = "xyes"])
fi

if test "x$arg_webdav" = "xno"; then
  AM_CONDITIONAL([NEON_FOUND], [false])
else
  PKG_CHECK_MODULES([NEON],
    [neon], [NEON_FOUND="yes"], [echo "$NEON_PKG_ERRORS"])
  AM_CONDITIONAL([NEON_FOUND], [test "x$NEON_FOUND" = "xyes"])
fi

if test "x$arg_python" = "xno" -o "x$arg_util" = "xno"; then
  AM_CONDITIONAL([BUILD_PPKLIB], [test "x$arg_ppklib" != "xno"])
else
  AM_CONDITIONAL([BUILD_PPKLIB], [true])
fi

AC_OUTPUT(
[Makefile
src/Makefile
src/gtk/Makefile
src/gtk/Ask_For_Pass/Makefile
src/qt/Makefile
src/qt/Ask_For_Pass/Makefile
src/kde/Makefile
src/kde/KWallet/Makefile
src/gnome/Makefile
src/gnome/GKeyring/Makefile
src/portable/Makefile
src/portable/SaveToFile/Makefile
src/portable/SaveToFile/encrypted/Makefile
src/portable/SaveToFile/plain_text/Makefile
src/windows/Makefile
src/windows/Ask_For_Pass/Makefile
src/windows/SaveToRegistry/Makefile
src/ppasskeeper.pc
python/Makefile
util/Makefile
util/qt/Makefile]
)

echo
echo -n "Build the ppasskeeper library : "
if test "x$arg_ppklib" = "xno"; then
  if test "x${PYTHON_FOUND}" = "xyes"; then
    echo "yes (required by the Python binding)"
  else
    if test "x$arg_util" != "xno"; then
      echo "yes (required by the command-line utility)"
    else
      echo "no"
    fi
  fi
else
  echo "yes"
fi

echo -n 'Debug output : '; test "x$arg_debug" = "xyes" && echo yes || echo no

echo
echo "Enabled optional modules :"
echo "-------------------------------------------"
echo -n "gtk : " ; test -z "$GTK2_FOUND" && echo "no" || echo "$GTK2_FOUND"
echo -n "kde : " ; test -z "$KDE4_INCLUDES" && echo "no" || echo "yes"
echo -n "gnome : " ; test -z "$GKEYRING_FOUND" && echo "no" || echo "yes"
echo -n "qt  : " ; echo "$BUILD_QT"
echo -n "webdav : "; test -z "$NEON_FOUND" && echo "no" || echo "yes"
echo -n "save_to_file : " ; test "x$arg_stf" = "xno" && echo "no" || echo "yes"
echo -n "win32 : " ; test -z "$WINDOWS_FOUND" && echo "no" || echo "$WINDOWS_FOUND"
echo
echo "Bindings :"
echo "-------------------------------------------"
echo -n "python binding : " ; test "x${PYTHON_FOUND}" = "xno" && echo "no" || echo "yes"
echo
echo "Utilities :"
echo "-------------------------------------------"
echo -n "ppasskeeper (command-line): " ; test "x$arg_util" = "xno" && echo "no" || echo "yes"
echo -n "Qt4 GUI utility: " ; echo "$BUILD_QT_GUI"
