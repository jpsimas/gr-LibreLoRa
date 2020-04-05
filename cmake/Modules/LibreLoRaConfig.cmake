INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_LIBRELORA LibreLoRa)

FIND_PATH(
    LIBRELORA_INCLUDE_DIRS
    NAMES LibreLoRa/api.h
    HINTS $ENV{LIBRELORA_DIR}/include
        ${PC_LIBRELORA_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    LIBRELORA_LIBRARIES
    NAMES gnuradio-LibreLoRa
    HINTS $ENV{LIBRELORA_DIR}/lib
        ${PC_LIBRELORA_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/LibreLoRaTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBRELORA DEFAULT_MSG LIBRELORA_LIBRARIES LIBRELORA_INCLUDE_DIRS)
MARK_AS_ADVANCED(LIBRELORA_LIBRARIES LIBRELORA_INCLUDE_DIRS)
