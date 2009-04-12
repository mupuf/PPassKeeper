CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE
	STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
	FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(FindPkgConfig)
INCLUDE(FindQt4)
#INCLUDE(FindKDE4)
INCLUDE(FindPythonLibs)
INCLUDE(FindPythonInterp)
INCLUDE(FeatureSummary)

OPTION(PPK_LIB "PPassKeeper library" ON)
OPTION(PPK_STF "SaveToFile plugins" ON)
OPTION(PPK_UTIL "PPassKeeper command-line utility" ON)
OPTION(PPK_UTIL_QT "PPassKeeper GUI utility" ON)
OPTION(PPK_PYTHON "Python binding" ON)
OPTION(PPK_GTK "GTK plugin" ON)
OPTION(PPK_GNOME "GNOME plugin" ON)
OPTION(PPK_KDE4 "KDE4 plugin" ON)
OPTION(PPK_QT "Qt plugin" ON)
OPTION(PPK_DEBUG "Debugging messages" OFF)
OPTION(PPK_ELEKTRA "Use LibElektra" ON)

IF(PPK_ELEKTRA)
  PKG_SEARCH_MODULE(elektra elektra)
  IF(elektra_FOUND)
    INCLUDE_DIRECTORIES(${elektra_INCLUDE_DIRS})
    LINK_DIRECTORIES(${elektra_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_ELEKTRA)
    MESSAGE("Params : Compile against libelektra")
  ELSE(elektra_FOUND)
    MESSAGE("Params : Libelektra not found ! Fallback to xml")
  ENDIF(elektra_FOUND)
ELSE(PPK_ELEKTRA)
  MESSAGE("Params : Forced to use the xml fallback instead of libelektra")
ENDIF(PPK_ELEKTRA)

IF(PPK_UTIL_QT OR PPK_QT)
  FIND_PACKAGE(Qt4)
  IF(NOT QT4_FOUND)
	SET(PPK_UTIL_QT OFF)
	SET(PPK_QT OFF)
  ELSE(NOT QT4_FOUND)
	INCLUDE(${QT_USE_FILE})
  ENDIF(NOT QT4_FOUND)
ENDIF(PPK_UTIL_QT OR PPK_QT)

IF(PPK_PYTHON)
  FIND_PACKAGE(PythonLibs)
  IF(NOT PYTHONLIBS_FOUND)
	SET(PPK_PYTHON OFF)
  ELSE(NOT PYTHONLIBS_FOUND)
	INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
	FIND_PACKAGE(PythonInterp)
	IF(NOT PYTHONINTERP_FOUND)
	  SET(PPK_PYTHON OFF)
	ELSE(NOT PYTHONINTERP_FOUND)
	  EXECUTE_PROCESS(
		COMMAND "${PYTHON_EXECUTABLE}" -c "import distutils.sysconfig; print distutils.sysconfig.get_python_lib();"
		RESULT_VARIABLE PYTHON_LIB_RESULT
		OUTPUT_VARIABLE PYTHON_LIB_PATH
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	  IF(PYTHON_LIB_RESULT EQUAL 0)
		MESSAGE("Python library found in ${PYTHON_LIB_PATH}")
	  ELSE(PYTHON_LIB_RESULT EQUAL 0)
		SET(PPK_PYTHON OFF)
	  ENDIF(PYTHON_LIB_RESULT EQUAL 0)
	ENDIF(NOT PYTHONINTERP_FOUND)
  ENDIF(NOT PYTHONLIBS_FOUND)
ENDIF(PPK_PYTHON)

IF(PPK_GTK)
  PKG_SEARCH_MODULE(gtk2 gtk+-2.0)
  IF(NOT gtk2_FOUND)
	SET(PPK_GTK OFF)
  ELSE(NOT gtk2 FOUND)
	INCLUDE_DIRECTORIES(${gtk2_INCLUDE_DIRS})
	LINK_DIRECTORIES(${gtk2_LIBRARY_DIRS})	
  ENDIF(NOT gtk2_FOUND)
ENDIF(PPK_GTK)

IF(PPK_GNOME)
  PKG_SEARCH_MODULE(gkeyring gnome-keyring-1)
  IF(NOT gkeyring_FOUND)
	SET(PPK_GNOME OFF)
  ELSE(NOT gkeyring_FOUND)
	INCLUDE_DIRECTORIES(${gkeyring_INCLUDE_DIRS})
	LINK_DIRECTORIES(${gkeyring_LIBRARY_DIRS})	
  ENDIF(NOT gkeyring_FOUND)
ENDIF(PPK_GNOME)

IF(PPK_KDE4)
  FIND_LIBRARY(KWALLET_LIB kdeinit4_kwalletd)
  IF(NOT KWALLET_LIB)
    SET(PPK_KDE4 OFF)
  ENDIF(NOT KWALLET_LIB)
ENDIF(PPK_KDE4)

IF(PPK_DEBUG)
  ADD_DEFINITIONS(-DDEBUG_MSG)
ENDIF(PPK_DEBUG)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
SET(PPK_DIRECTORY_PATH ${CMAKE_INSTALL_PREFIX}/lib/ppasskeeper)
ADD_DEFINITIONS(-DDIRECTORY_PATH="${PPK_DIRECTORY_PATH}")

# Header and library checks
ADD_DEFINITIONS(-DHAVE_CONFIG_H=1)
CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(stdbool.h HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(syscall.h HAVE_SYSCALL_H)
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_BINARY_DIR})
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

CHECK_FUNCTION_EXISTS(chmod HAVE_CHMOD)
CHECK_LIBRARY_EXISTS(dl dlopen “” HAVE_LIBDL)
IF(HAVE_CHMOD AND HAVE_SYSCALL_H)
  ADD_DEFINITIONS(-DHAVE_CHMOD=1)
ENDIF(HAVE_CHMOD AND HAVE_SYSCALL_H)

# PPK library
IF(PPK_LIB)
  FILE(GLOB libppasskeeper_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.[ch]" "src/*.cpp")
  ADD_LIBRARY(ppasskeeper SHARED ${libppasskeeper_SOURCES})
  IF(HAVE_LIBDL)
	TARGET_LINK_LIBRARIES(ppasskeeper "dl")
  ENDIF(HAVE_LIBDL)
  SET_PROPERTY(TARGET ppasskeeper PROPERTY VERSION 1.0.0)
  SET_PROPERTY(TARGET ppasskeeper PROPERTY SOVERSION 1)
  TARGET_LINK_LIBRARIES(ppasskeeper ${elektra_LIBRARIES})
  INSTALL(TARGETS ppasskeeper DESTINATION lib)
  INSTALL(FILES "${CMAKE_SOURCE_DIR}/src/ppasskeeper.h" DESTINATION include)
ENDIF(PPK_LIB)

# SaveToFile plugins
IF(PPK_STF)
  FILE(GLOB ppk_savetofile_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"src/portable/SaveToFile/*.[ch]"
	"src/portable/SaveToFile/*.cpp")
  ADD_LIBRARY(ppk_savetofile_pt MODULE
	${ppk_savetofile_SOURCES}
	"src/portable/SaveToFile/plain_text/savetofile_plain_text.cpp")
  ADD_LIBRARY(ppk_savetofile_enc MODULE
	${ppk_savetofile_SOURCES}
	"src/portable/SaveToFile/encrypted/base64.cpp"
	"src/portable/SaveToFile/encrypted/base64.h"
	"src/portable/SaveToFile/encrypted/savetofile_encrypted.cpp")
  INSTALL(TARGETS ppk_savetofile_pt ppk_savetofile_enc DESTINATION lib/ppasskeeper)
ENDIF(PPK_STF)

# Command-line utility
IF(PPK_UTIL)
  ADD_EXECUTABLE(ppasskeeper-util "util/ppasskeeper-util.c")
  TARGET_LINK_LIBRARIES(ppasskeeper-util "ppasskeeper")
  SET_PROPERTY(TARGET ppasskeeper-util PROPERTY OUTPUT_NAME "ppasskeeper")
  INSTALL(TARGETS ppasskeeper-util DESTINATION bin)
  INSTALL(FILES "${CMAKE_SOURCE_DIR}/util/ppasskeeper.1" DESTINATION share/man/man1)
ENDIF(PPK_UTIL)

# Qt Utility
IF(PPK_UTIL_QT)
  FILE(GLOB ppasskeeper-qt_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"util/qt/*.h"
	"util/qt/*.cpp")
  SET(ppasskeeper-qt_MOCS mainwindow.h passwordlistmodel.h)
  SET(ppasskeeper-qt_UIS mainwindow.ui)
  FOREACH(MOC ${ppasskeeper-qt_MOCS})
	QT4_WRAP_CPP(ppasskeeper-qt_SOURCES "util/qt/${MOC}")
  ENDFOREACH(MOC ${ppasskeeper-qt_MOCS})
  FOREACH(UI ${ppasskeeper-qt_UIS})
	QT4_WRAP_UI(ppasskeeper-qt_SOURCES "util/qt/${UI}")
  ENDFOREACH(UI ${ppasskeeper-qt_UIS})
  ADD_EXECUTABLE(ppasskeeper-qt ${ppasskeeper-qt_SOURCES})
  TARGET_LINK_LIBRARIES(ppasskeeper-qt ${QT_LIBRARIES} ppasskeeper)
  INSTALL(TARGETS ppasskeeper-qt DESTINATION bin)
  INSTALL(FILES "${CMAKE_SOURCE_DIR}/util/qt/ppasskeeper-qt.desktop" DESTINATION share/applications)
ENDIF(PPK_UTIL_QT)

# Python binding
IF(PPK_PYTHON)
  FILE(GLOB ppk_python_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"python/*.[ch]")
  ADD_LIBRARY(ppk_python MODULE ${ppk_python_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_python ${PYTHON_LIBRARIES} ppasskeeper)
  SET_PROPERTY(TARGET ppk_python PROPERTY OUTPUT_NAME "ppk")
  SET_PROPERTY(TARGET ppk_python PROPERTY PREFIX "")
  INSTALL(TARGETS ppk_python DESTINATION "${PYTHON_LIB_PATH}")
ENDIF(PPK_PYTHON)

# GTK plugin
IF(PPK_GTK)
  FILE(GLOB ppk_gtk_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"src/gtk/Ask_For_Pass/*.[ch]"
	"src/gtk/Ask_For_Pass/*.cpp")
   ADD_LIBRARY(ppk_gtk MODULE ${ppk_gtk_SOURCES})
   TARGET_LINK_LIBRARIES(ppk_gtk ${gtk2_LIBRARIES})
   INSTALL(TARGETS ppk_gtk DESTINATION lib/ppasskeeper)
ENDIF(PPK_GTK)

# GNOME plugin
IF(PPK_GNOME)
  FILE(GLOB ppk_gnome_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"src/gnome/GKeyring/*.[ch]"
	"src/gnome/GKeyring/*.cpp")
  ADD_LIBRARY(ppk_gnome MODULE ${ppk_gnome_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_gnome ${gkeyring_LIBRARIES})
  INSTALL(TARGETS ppk_gnome DESTINATION lib/ppasskeeper)
ENDIF(PPK_GNOME)

# KDE4 plugin
IF(PPK_KDE4)
  FILE(GLOB ppk_kde4_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"src/kde/KWallet/*.[ch]"
	"src/kde/KWallet/*.cpp")
  ADD_LIBRARY(ppk_kwallet MODULE ${ppk_kde4_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_kwallet "kdeinit4_kwalletd")
  INSTALL(TARGETS ppk_kwallet DESTINATION lib/ppasskeeper)
ENDIF(PPK_KDE4)

# Qt plugin
IF(PPK_QT)
  FILE(GLOB ppk_qt_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"src/qt/Ask_For_Pass/*.[ch]"
	"src/qt/Ask_For_Pass/*.cpp")
  ADD_LIBRARY(ppk_qt MODULE ${ppk_qt_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_qt ${QT_LIBRARIES})
  INSTALL(TARGETS ppk_qt DESTINATION lib/ppasskeeper)
ENDIF(PPK_QT)

#PKG-CONFIG
set(PPK_VERSION "1.0b2")
configure_file(ppasskeeper.pc.in ppasskeeper.pc @ONLY)
install(FILES ppasskeeper.pc DESTINATION lib/pkgconfig)

