# CMake version >= 3.8.2
# This requirement is for the "cxx_std_11" variable in target_compile_features().
cmake_minimum_required(VERSION 3.8.2)
project(libmorton VERSION 0.2.7)

option(BUILD_TESTING "Build unit tests for libmorton" ON)

set(libmorton_INCLUDE_DIR "${libmorton_SOURCE_DIR}")

include(GNUInstallDirs)
set(libmorton_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(libmorton_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(libmorton_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_DATADIR}/cmake/libmorton")
set(libmorton_INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_DATADIR}/pkgconfig")

add_library(libmorton INTERFACE)

set_property(TARGET libmorton
  PROPERTY
    PUBLIC_HEADER
      "${libmorton_SOURCE_DIR}/libmorton/morton_common.h"
      "${libmorton_SOURCE_DIR}/libmorton/morton_AVX512BITALG.h"
      "${libmorton_SOURCE_DIR}/libmorton/morton_BMI.h"
      "${libmorton_SOURCE_DIR}/libmorton/morton2D_LUTs.h"
      "${libmorton_SOURCE_DIR}/libmorton/morton2D.h"
      "${libmorton_SOURCE_DIR}/libmorton/morton3D_LUTs.h"
      "${libmorton_SOURCE_DIR}/libmorton/morton3D.h"
      "${libmorton_SOURCE_DIR}/libmorton/morton.h"
  )

target_include_directories(libmorton
  INTERFACE
    $<BUILD_INTERFACE:${libmorton_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${libmorton_INSTALL_INCLUDE_DIR}>
  )

add_library(libmorton::libmorton ALIAS libmorton)

if(BUILD_TESTING)
  enable_testing()

  add_subdirectory(test)
endif()

install(TARGETS libmorton
  EXPORT
    libmortonTargets
  PUBLIC_HEADER
    DESTINATION
      "${libmorton_INSTALL_INCLUDE_DIR}/libmorton"
  )

export(TARGETS libmorton
  NAMESPACE libmorton::
  FILE
    "${libmorton_BINARY_DIR}/libmortonTargets.cmake"
  )
export(PACKAGE libmorton)
install(EXPORT libmortonTargets
  NAMESPACE libmorton::
  DESTINATION
    "${libmorton_INSTALL_CMAKE_DIR}"
  )

include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${libmorton_SOURCE_DIR}/data/libmortonConfig.cmake.in"
  "${libmorton_BINARY_DIR}/libmortonConfig.cmake"
  INSTALL_DESTINATION
    "${libmorton_INSTALL_CMAKE_DIR}"
  PATH_VARS
    libmorton_INSTALL_INCLUDE_DIR
  )
write_basic_package_version_file(
  "${libmorton_BINARY_DIR}/libmortonConfigVersion.cmake"
  VERSION ${libmorton_VERSION}
  ARCH_INDEPENDENT
  COMPATIBILITY
    SameMajorVersion
  )
install(
  FILES
    "${libmorton_BINARY_DIR}/libmortonConfigVersion.cmake"
    "${libmorton_BINARY_DIR}/libmortonConfig.cmake"
  DESTINATION
    "${libmorton_INSTALL_CMAKE_DIR}"
  )

configure_file(
  "${libmorton_SOURCE_DIR}/data/libmorton.pc.in"
  "${libmorton_BINARY_DIR}/libmorton.pc" @ONLY
  )
install(
  FILES
    "${libmorton_BINARY_DIR}/libmorton.pc"
  DESTINATION
    "${libmorton_INSTALL_PKGCONFIG_DIR}"
  )
