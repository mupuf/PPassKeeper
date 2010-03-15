CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE
	STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
	FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_PROJECT_NAME "PPassKeeper")

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(FindPkgConfig)
INCLUDE(FindQt4)
#INCLUDE(FindKDE4)
INCLUDE(FindPythonLibs)
INCLUDE(FindPythonInterp)
INCLUDE(FeatureSummary)

INCLUDE_DIRECTORIES("include/")

OPTION(PPK_LIB "PPassKeeper library" ON)
OPTION(PPK_BUILD_PLUGINS "Build plugins" ON)
OPTION(PPK_STF "SaveToFile plugins" ON)
OPTION(PPK_STR "SaveToRegistry module (WIN32)" ON)
OPTION(PPK_AFP_GTK "Ask For Password GTK plugin" ON)
OPTION(PPK_AFP_QT "Ask For Password Qt plugin" ON)
OPTION(PPK_AFP_WIN32 "Ask For Password Win32 plugin" ON)
OPTION(PPK_GNOMEKEYRING "GNOME Keyring plugin" ON)
OPTION(PPK_KWALLET4 "KWallet4 plugin" ON)
OPTION(PPK_ELEKTRA "Use LibElektra" ON)
OPTION(PPK_PYTHON "Python binding" ON)
#OPTION(PPK_PYTHON_SITE_PACKAGES_DIR "Python site-packages directory" OFF)
OPTION(PPK_DEBUG "Debugging messages" OFF)
OPTION(PPK_UTIL "PPassKeeper command-line utility" ON)
OPTION(PPK_UTIL_QT "PPassKeeper GUI utility" ON)
OPTION(PPK_TESTS "Build the unit tests" OFF)

IF(NOT PPK_BUILD_PLUGINS)
  SET(PPK_STF OFF)
  SET(PPK_STR OFF)
  SET(PPK_AFP_QT OFF)
  SET(PPK_AFP_GTK OFF)
  SET(PPK_AFP_WIN32 OFF)
  SET(PPK_GNOMEKEYRING OFF)
  SET(PPK_KWALLET4 OFF)
ENDIF(NOT PPK_BUILD_PLUGINS)

ADD_DEFINITIONS("-Wall")

IF(PPK_UTIL_QT OR PPK_AFP_QT)
  FIND_PACKAGE(Qt4)
  IF(NOT QT4_FOUND)
	SET(PPK_UTIL_QT OFF)
	SET(PPK_AFP_QT OFF)
  ELSE(NOT QT4_FOUND)
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(-DQT_NO_CAST_FROM_ASCII)
	ADD_DEFINITIONS(-DQT_NO_CAST_TO_ASCII)
  ENDIF(NOT QT4_FOUND)
ENDIF(PPK_UTIL_QT OR PPK_AFP_QT)

IF(PPK_PYTHON)
	FIND_PACKAGE(PythonLibs)
	IF(NOT PYTHONLIBS_FOUND)
		SET(PPK_PYTHON OFF)
	ELSE(NOT PYTHONLIBS_FOUND)
		find_package(Boost COMPONENTS python)
		if(Boost_FOUND)
			INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
			FIND_PACKAGE(PythonInterp)
			IF(NOT PYTHONINTERP_FOUND)
				SET(PPK_PYTHON OFF)
				ELSE(NOT PYTHONINTERP_FOUND)
				IF(PPK_PYTHON_SITE_PACKAGES_DIR)
					SET(PYTHON_LIB_PATH "${PPK_PYTHON_SITE_PACKAGES_DIR}")
				ELSE(PPK_PYTHON_SITE_PACKAGES_DIR)
					EXECUTE_PROCESS(
					COMMAND "${PYTHON_EXECUTABLE}" -c "import distutils.sysconfig; print distutils.sysconfig.get_python_lib();"
					RESULT_VARIABLE PYTHON_LIB_RESULT
					OUTPUT_VARIABLE PYTHON_LIB_PATH
					OUTPUT_STRIP_TRAILING_WHITESPACE)
					IF(PYTHON_LIB_RESULT EQUAL 0)
						MESSAGE("Python library found in ${PYTHON_LIB_PATH}")
						STRING(REGEX REPLACE "^.*lib/python" "lib/python" PYTHON_LIB_PATH "${PYTHON_LIB_PATH}")
					ELSE(PYTHON_LIB_RESULT EQUAL 0)
						SET(PPK_PYTHON OFF)
					ENDIF(PYTHON_LIB_RESULT EQUAL 0)
				ENDIF(PPK_PYTHON_SITE_PACKAGES_DIR)
				ENDIF(NOT PYTHONINTERP_FOUND)
		ELSE(Boost_FOUND)
			SET(PPK_PYTHON OFF)
		ENDIF(Boost_FOUND)
	ENDIF(NOT PYTHONLIBS_FOUND)
ENDIF(PPK_PYTHON)

IF(NOT CMAKE_HOST_WIN32)
  SET(PPK_AFP_WIN32 OFF)
  SET(PPK_STR OFF)
ENDIF(NOT CMAKE_HOST_WIN32)

IF(PPK_AFP_GTK)
  PKG_SEARCH_MODULE(gtk2 gtk+-2.0)
  IF(NOT gtk2_FOUND)
	SET(PPK_AFP_GTK OFF)
  ELSE(NOT gtk2 FOUND)
	INCLUDE_DIRECTORIES(${gtk2_INCLUDE_DIRS})
	LINK_DIRECTORIES(${gtk2_LIBRARY_DIRS})
  ENDIF(NOT gtk2_FOUND)
ENDIF(PPK_AFP_GTK)

IF(PPK_GNOMEKEYRING)
  PKG_SEARCH_MODULE(gkeyring gnome-keyring-1)
  IF(NOT gkeyring_FOUND)
	SET(PPK_GNOMEKEYRING OFF)
  ELSE(NOT gkeyring_FOUND)
	INCLUDE_DIRECTORIES(${gkeyring_INCLUDE_DIRS})
	LINK_DIRECTORIES(${gkeyring_LIBRARY_DIRS})
  ENDIF(NOT gkeyring_FOUND)
ENDIF(PPK_GNOMEKEYRING)

IF(PPK_KWALLET4)
  FIND_LIBRARY(KWALLET_LIB kdeinit4_kwalletd)
  IF(NOT KWALLET_LIB)
    SET(PPK_KWALLET4 OFF)
  ELSE(NOT KWALLET_LIB)
    FIND_PROGRAM(KDE4CONFIG NAMES kde4-config kde-config)
    IF(KDE4CONFIG)
      EXECUTE_PROCESS(COMMAND "${KDE4CONFIG}" --path include OUTPUT_VARIABLE KDE4_PATH_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
      EXECUTE_PROCESS(COMMAND "${KDE4CONFIG}" --path lib OUTPUT_VARIABLE KDE4_PATH_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
      INCLUDE_DIRECTORIES(${KDE4_PATH_INCLUDE})
      LINK_DIRECTORIES(${KDE4_PATH_LIB})
    ELSE(KDE4CONFIG)
      SET(PPK_KWALLET4 OFF)
    ENDIF(KDE4CONFIG)
  ENDIF(NOT KWALLET_LIB)
ENDIF(PPK_KWALLET4)

#Lib Elektra
IF(PPK_ELEKTRA)
  PKG_SEARCH_MODULE(elektra elektra)
  IF(elektra_FOUND)
    INCLUDE_DIRECTORIES(${elektra_INCLUDE_DIRS})
    LINK_DIRECTORIES(${elektra_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_ELEKTRA)
  ELSE(elektra_FOUND)
    SET(PPK_ELEKTRA OFF)
  ENDIF(elektra_FOUND)
ENDIF(PPK_ELEKTRA)

IF(PPK_DEBUG)
  ADD_DEFINITIONS(-DDEBUG_MSG)
ENDIF(PPK_DEBUG)

#Tell PPK where to find the plugins
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
INCLUDE_DIRECTORIES(BEFORE "${CMAKE_SOURCE_DIR}/include")
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
  INSTALL(FILES "${CMAKE_SOURCE_DIR}/include/ppasskeeper.h" DESTINATION include)
  INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/include/ppasskeeper" DESTINATION include PATTERN ".svn" EXCLUDE)
ENDIF(PPK_LIB)

# SaveToFile plugins
IF(PPK_STF)
  FILE(GLOB ppk_savetofile_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"modules/portable/SaveToFile/*.[ch]"
	"modules/portable/SaveToFile/*.cpp"
	"modules/ppasskeeper-dir.cpp")
  ADD_LIBRARY(ppk_savetofile_pt MODULE
	${ppk_savetofile_SOURCES}
	"modules/portable/SaveToFile/plain_text/savetofile_plain_text.cpp")
  ADD_LIBRARY(ppk_savetofile_enc MODULE
	${ppk_savetofile_SOURCES}
	"modules/portable/SaveToFile/encrypted/base64.c"
	"modules/portable/SaveToFile/encrypted/base64.h"
	"modules/portable/SaveToFile/encrypted/savetofile_encrypted.cpp")

  TARGET_LINK_LIBRARIES(ppk_savetofile_pt ppasskeeper)
  TARGET_LINK_LIBRARIES(ppk_savetofile_enc ppasskeeper)

  INSTALL(TARGETS ppk_savetofile_pt ppk_savetofile_enc DESTINATION lib/ppasskeeper)
ENDIF(PPK_STF)

# GTK plugin
IF(PPK_AFP_GTK)
  FILE(GLOB ppk_afp_gtk_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"modules/gtk/AskForPass/*.[ch]"
	"modules/gtk/AskForPass/*.cpp")
   ADD_LIBRARY(ppk_afp_gtk MODULE ${ppk_afp_gtk_SOURCES})
   TARGET_LINK_LIBRARIES(ppk_afp_gtk ${gtk2_LIBRARIES} ppasskeeper)
   INSTALL(TARGETS ppk_afp_gtk DESTINATION lib/ppasskeeper)
ENDIF(PPK_AFP_GTK)

# Qt plugin
IF(PPK_AFP_QT)
  FILE(GLOB ppk_afp_qt_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"modules/qt/AskForPass/*.[ch]"
	"modules/qt/AskForPass/*.cpp")
  ADD_LIBRARY(ppk_afp_qt MODULE ${ppk_afp_qt_SOURCES})

  TARGET_LINK_LIBRARIES(ppk_afp_qt ${QT_LIBRARIES} ppasskeeper)

  INSTALL(TARGETS ppk_afp_qt DESTINATION lib/ppasskeeper)
ENDIF(PPK_AFP_QT)

#Win32 plugin
IF(PPK_AFP_WIN32)
  FILE(GLOB ppk_afp_win32_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"modules/windows/AskForPass/*.[ch]"
	"modules/windows/AskForPass/*.cpp")
  ADD_LIBRARY(ppk_afp_win32 MODULE ${ppk_afp_win32_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_afp_win32 ppasskeeper)
  INSTALL(TARGETS ppk_afp_win32 DESTINATION lib/ppasskeeper)
ENDIF(PPK_AFP_WIN32)

# GNOME plugin
IF(PPK_GNOMEKEYRING)
  FILE(GLOB ppk_gnomekeyring_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"modules/gnome/GKeyring/*.[ch]"
	"modules/gnome/GKeyring/*.cpp")
  ADD_LIBRARY(ppk_gnomekeyring MODULE ${ppk_gnomekeyring_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_gnomekeyring ${gkeyring_LIBRARIES} ppasskeeper)
  INSTALL(TARGETS ppk_gnomekeyring DESTINATION lib/ppasskeeper)
ENDIF(PPK_GNOMEKEYRING)

# KDE4 plugin
IF(PPK_KWALLET4)
  FILE(GLOB ppk_kwallet4_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"modules/kde/KWallet/*.[ch]"
	"modules/kde/KWallet/*.cpp")
  ADD_LIBRARY(ppk_kwallet MODULE ${ppk_kwallet4_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_kwallet "kdeinit4_kwalletd" ppasskeeper)
  INSTALL(TARGETS ppk_kwallet DESTINATION lib/ppasskeeper)
ENDIF(PPK_KWALLET4)

#Win32 module
IF(PPK_STR)
  FILE(GLOB ppk_savetoregistry_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"modules/windows/SaveToRegistry/*.[ch]"
	"modules/windows/SaveToRegistry/*.cpp")
  ADD_LIBRARY(ppk_savetoregistry MODULE ${ppk_savetoregistry_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_savetoregistry ppasskeeper)
  INSTALL(TARGETS ppk_savetoregistry DESTINATION lib/ppasskeeper)
ENDIF(PPK_STR)

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
	"util/qt/*.cpp"
	"util/qt/form_fields/*.h"
	"util/qt/form_fields/*.cpp"
      )
  FILE(GLOB ppasskeeper-qt_ICONS RELATIVE ${CMAKE_SOURCE_DIR}
	"icons/ppk_logo_mini*"
      )

  SET(ppasskeeper-qt_MOCS addpwd.h mainwindow.h passwordlistmodel.h infomodule.h editparams.h form_fields/qabstractformfield.h form_fields/qdoublespinfield.h form_fields/qspinfield.h form_fields/qtextfield.h form_fields/qfilefield.h form_fields/qdirfield.h form_fields/qcombofield.h form_fields/qmodulefield.h)
  SET(ppasskeeper-qt_UIS addpwd.ui mainwindow.ui infomodule.ui editparams.ui)
  SET(ppasskeeper-qt_RCC "util/qt/ressources.qrc")

  QT4_ADD_RESOURCES(ppasskeeper-qt_RCC_SRCS ${ppasskeeper-qt_RCC})
  FOREACH(MOC ${ppasskeeper-qt_MOCS})
	QT4_WRAP_CPP(ppasskeeper-qt_SOURCES "util/qt/${MOC}")
  ENDFOREACH(MOC ${ppasskeeper-qt_MOCS})
  FOREACH(UI ${ppasskeeper-qt_UIS})
	QT4_WRAP_UI(ppasskeeper-qt_SOURCES "util/qt/${UI}")
  ENDFOREACH(UI ${ppasskeeper-qt_UIS})
  ADD_EXECUTABLE(ppasskeeper-qt WIN32 MACOSX_BUNDLE ${ppasskeeper-qt_SOURCES} ${ppasskeeper-qt_RCC_SRCS})
  TARGET_LINK_LIBRARIES(ppasskeeper-qt ${QT_LIBRARIES} ppasskeeper)
  INSTALL(TARGETS ppasskeeper-qt DESTINATION bin)
  INSTALL(FILES "${CMAKE_SOURCE_DIR}/util/qt/ppasskeeper-qt.desktop" DESTINATION share/applications)
  INSTALL(FILES ${ppasskeeper-qt_ICONS} DESTINATION share/pixmaps)
ENDIF(PPK_UTIL_QT)

# Python binding
#  FILE(GLOB ppk_python_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
#	"python/*.[ch]")
#  ADD_LIBRARY(ppk_python MODULE ${ppk_python_SOURCES})
#  TARGET_LINK_LIBRARIES(ppk_python ${PYTHON_LIBRARIES} ppasskeeper)
#  SET_PROPERTY(TARGET ppk_python PROPERTY OUTPUT_NAME "ppk")
#  SET_PROPERTY(TARGET ppk_python PROPERTY PREFIX "")
#  INSTALL(TARGETS ppk_python DESTINATION "${PYTHON_LIB_PATH}")
IF(PPK_PYTHON)
  FILE(GLOB ppk_python_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
	"python/*.cpp")
  ADD_LIBRARY(ppk_python MODULE ${ppk_python_SOURCES})
  TARGET_LINK_LIBRARIES(ppk_python ppasskeeper ${Boost_LIBRARIES})
  SET_PROPERTY(TARGET ppk_python PROPERTY OUTPUT_NAME "ppasskeeper")
  SET_PROPERTY(TARGET ppk_python PROPERTY PREFIX "")
  INSTALL(TARGETS ppk_python DESTINATION "${PYTHON_LIB_PATH}")
ENDIF(PPK_PYTHON)

#PKG-CONFIG
set(PPK_VERSION "1.0b2")
configure_file(ppasskeeper.pc.in ppasskeeper.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ppasskeeper.pc" DESTINATION lib/pkgconfig)

IF(PPK_TESTS)
  INCLUDE_DIRECTORIES(BEFORE "${CMAKE_SOURCE_DIR}/include")
  LINK_DIRECTORIES(BEFORE "${CMAKE_SOURCE_DIR}/src")

  SET(UNITTESTS "" ) #"get-set-blob" "get-set-string"

  FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/unittest")

  FOREACH(UNITTEST ${UNITTESTS})
    FILE(GLOB UNITTEST_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "unittest/${UNITTEST}/*.[ch]" "unittest/${UNITTEST}/*.cpp")
    ADD_EXECUTABLE("unittest/${UNITTEST}" "${UNITTEST_SOURCES}")
    TARGET_LINK_LIBRARIES("unittest/${UNITTEST}" ppasskeeper)
  ENDFOREACH()
ENDIF(PPK_TESTS)

#PACKAGES
	#GENERAL
	SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")
	SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
	SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PPassKeeper is a library that intends to create a portable way of storing sensitive information.")
	SET(CPACK_PACKAGE_EXECUTABLES "ppasskeeper-qt;PPassKeeper Backends Manager")
	SET(CPACK_PACKAGE_FILE_NAME "ppasskeeper-1.0.0-b2")
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "PPassKeeper 1.0 beta 2")
	SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "PPassKeeper 1.0 beta 2")
	SET(CPACK_PACKAGE_NAME "PPassKeeper")
	SET(CPACK_PACKAGE_VENDOR "MùPùF.org")
	SET(CPACK_PACKAGE_VERSION "1.0.0b2")
	SET(CPACK_PACKAGE_VERSION_MAJOR "1")
	SET(CPACK_PACKAGE_VERSION_MINOR "0")
	SET(CPACK_PACKAGE_VERSION_PATCH "0")
	SET(CPACK_STRIP_FILES "")
	SET(CPACK_GENERATOR "TGZ;DEB;RPM;BUNDLE")

	SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
	SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
	SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")

	#DEBIAN
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Martin PERES <martin.peres@free.fr>")
	SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PPassKeeper is a library that intends to create a portable way of storing sensitive information. It is developed under the LGPL v2-or-later license.")
	SET(CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
	SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")
	SET(DEBIAN_PACKAGE_RECOMMENDS "libqtgui4 (>=4.0.0-1)")
	SET(DEBIAN_PACKAGE_SUGGESTS "gnome-keyring, kdebase-runtime (>=4:4.0.0-1), python")
	SET(DEBIAN_PACKAGE_BUILDS_DEPENDS "cmake (>=2.6), kdeutils-dev (>=4:4.0.0-1), libqt4-dev, libgtk2.0-dev libgnome-keyring-dev kdelibs5-dev libboost-python-dev")

	#RPM
	SET(CPACK_RPM_PACKAGE_RELEASE "1")
	SET(CPACK_RPM_PACKAGE_GROUP "MùPùF.org")
	SET(CPACK_RPM_PACKAGE_LICENSE "LGPL v2+")
	SET(CPACK_RPM_PACKAGE_REQUIRES "qt >= 4.0")

	#Binary TGZ
	SET(CPACK_PACKAGE_CONTACT "http://ppasskeeper.mupuf.org")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Martin Peres<martin.peres at ensi-bourges.fr>")
	#SET(CPACK_RESOURCE_FILE_WELCOME "")
	SET(CPACK_SOURCE_GENERATOR "TGZ")
	SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "CPackSourceConfig.cmake")
	SET(CPACK_SOURCE_PACKAGE_FILE_NAME "ppasskeeper-1.0.0-b2")
	SET(CPACK_SOURCE_STRIP_FILES "")

INCLUDE(CPack)

#Final summary
MESSAGE("")
MESSAGE("")
MESSAGE(" -------------------")
MESSAGE("|                   |")
MESSAGE("|      Summary      |")
MESSAGE("|                   |")
MESSAGE(" -------------------")
MESSAGE("")

MESSAGE("Here is the summary of what will be compiled (ON) or not(OFF).")
MESSAGE("If you didn't deactivate one plugin but still get an OFF on it, then the dependencies of the plugin are not met.")
MESSAGE("")
MESSAGE("")

MESSAGE("Library : ${PPK_LIB} (-DPPK_LIB=OFF to deactivate)")
MESSAGE("	Using LibElektra for saving parameters: ${PPK_ELEKTRA} (-DPPK_ELEKTRA=OFF to use the fallback XML system)")
MESSAGE("")

MESSAGE("Modules : ${PPK_BUILD_PLUGINS} (-DPPK_BUILD_PLUGINS=OFF to deactivate)")
MESSAGE("	SaveToFile : ${PPK_STF} (-DPPK_STF=OFF to deactivate)")
MESSAGE("	Ask For Password GTK : ${PPK_AFP_GTK} (-DPPK_AFP_GTK=OFF to deactivate)")
MESSAGE("	Ask For Password Qt : ${PPK_AFP_QT} (-DPPK_AFP_QT=OFF to deactivate)")
MESSAGE("	Ask For Password Win32 : ${PPK_AFP_WIN32} (-DPPK_AFP_WIN32=OFF to deactivate)")
MESSAGE("	Gnome Keyring : ${PPK_GNOMEKEYRING} (-DPPK_GNOME=OFF to deactivate)")
MESSAGE("	KDE4 Wallet : ${PPK_KWALLET4} (-DPPK_KWALLET4=OFF to deactivate)")
MESSAGE("	SaveToRegisry (Win32) : ${PPK_STR} (-DPPK_STR=OFF to deactivate)")
MESSAGE("")

MESSAGE("Bindings :")
MESSAGE("	Python : ${PPK_PYTHON} (-DPPK_PYTHON=OFF to deactivate)")
IF(PPK_PYTHON)
    MESSAGE("	Destination directory: ${PYTHON_LIB_PATH}")
ENDIF(PPK_PYTHON)
MESSAGE("")

MESSAGE("Utils :")
MESSAGE("	CLI : ${PPK_UTIL} (-DPPK_UTIL=OFF to deactivate)")
MESSAGE("	GUI (based on Qt) : ${PPK_UTIL_QT} (-DPPK_UTIL_QT=OFF to deactivate)")
MESSAGE("")


MESSAGE("Options :")
MESSAGE("	Debug Mode : ${PPK_DEBUG} (-DPPK_DEBUG=ON to activate)")
MESSAGE("	Compilation Mode : ${CMAKE_BUILD_TYPE} (-DCMAKE_BUILD_TYPE=Debug to debug)")
MESSAGE("")

MESSAGE("Unit Tests : ${PPK_TESTS} (-DPPK_TESTS=ON to activate)")
MESSAGE("")

MESSAGE("Install :")
MESSAGE("	Prefix : '${CMAKE_INSTALL_PREFIX}' (-DCMAKE_INSTALL_PREFIX=/your/prefix to modify the prefix to /your/prefix)")
MESSAGE("")

MESSAGE("OK, now, run :")
MESSAGE("$ make")
MESSAGE("# make install	<--This command will install ppk in '${CMAKE_INSTALL_PREFIX}'")
MESSAGE("")
MESSAGE("Have fun !")

MESSAGE("")
MESSAGE("")
