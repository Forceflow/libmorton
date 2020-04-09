# CMake version > 3.8.2
# This requirement is for the "cxx_std_11" variable in target_compile_features().
cmake_minimum_required(VERSION 3.8.2)
project(libmorton CXX)

option(LIBMORTON_BUILD_TESTS "Build unit tests for libmorton" ON)

add_library(libmorton INTERFACE)
target_compile_features(libmorton
  INTERFACE
    cxx_std_11
  )
target_compile_options(libmorton
  INTERFACE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
  )
target_compile_definitions(libmorton
  INTERFACE
    # To avoid errors related to std::min/std::max
    $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
  )
target_include_directories(libmorton
  INTERFACE
    libmorton/include
  )
add_subdirectory(libmorton)
add_library(libmorton::libmorton ALIAS libmorton)

if (LIBMORTON_BUILD_TESTS)
    add_subdirectory(test)
endif()