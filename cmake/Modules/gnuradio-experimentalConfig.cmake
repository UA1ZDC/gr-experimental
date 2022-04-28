find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_EXPERIMENTAL gnuradio-experimental)

FIND_PATH(
    GR_EXPERIMENTAL_INCLUDE_DIRS
    NAMES gnuradio/experimental/api.h
    HINTS $ENV{EXPERIMENTAL_DIR}/include
        ${PC_EXPERIMENTAL_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_EXPERIMENTAL_LIBRARIES
    NAMES gnuradio-experimental
    HINTS $ENV{EXPERIMENTAL_DIR}/lib
        ${PC_EXPERIMENTAL_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-experimentalTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_EXPERIMENTAL DEFAULT_MSG GR_EXPERIMENTAL_LIBRARIES GR_EXPERIMENTAL_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_EXPERIMENTAL_LIBRARIES GR_EXPERIMENTAL_INCLUDE_DIRS)
