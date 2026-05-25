find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_CYBERETHER gnuradio-cyberether)

FIND_PATH(
    GR_CYBERETHER_INCLUDE_DIRS
    NAMES gnuradio/cyberether/api.h
    HINTS $ENV{CYBERETHER_DIR}/include
        ${PC_CYBERETHER_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_CYBERETHER_LIBRARIES
    NAMES gnuradio-cyberether
    HINTS $ENV{CYBERETHER_DIR}/lib
        ${PC_CYBERETHER_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-cyberetherTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_CYBERETHER DEFAULT_MSG GR_CYBERETHER_LIBRARIES GR_CYBERETHER_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_CYBERETHER_LIBRARIES GR_CYBERETHER_INCLUDE_DIRS)
