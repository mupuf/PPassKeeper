CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE
	STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
	FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

PKG_SEARCH_MODULE(ppasskeeper REQUIRED ppasskeeper)

# BLOB
FILE(GLOB utstblob_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "blob/*.[ch]" "blob/*.cpp")
INCLUDE_DIRECTORIES(${ppasskeeper_INCLUDE_DIRS})
LINK_DIRECTORIES(${ppasskeeper_LIBRARY_DIRS})
ADD_EXECUTABLE(utstblob "${utstblob_SOURCES}")
TARGET_LINK_LIBRARIES(utstblob ${ppasskeeper_LIBRARIES})
MESSAGE("ppasskeeper_LIBRARIES = ${ppasskeeper_LIBRARIES}")
MESSAGE("ppasskeeper_LIBRARY_DIRS = ${ppasskeeper_LIBRARY_DIRS}")
