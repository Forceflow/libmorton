project(libmorton-tests LANGUAGES CXX)

add_executable(libmorton-test
  "libmorton_test.cpp"
  )
target_compile_options(libmorton-test
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: # GCC and clang
      -Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Intel>: # Intel
      $<IF:$<PLATFORM_ID:Windows>, /W3, -w3>>
    $<$<CXX_COMPILER_ID:MSVC>: # MSVC
      /W3>
  )
target_link_libraries(libmorton-test
  PRIVATE
    libmorton::libmorton
  )
add_test(libmorton-test libmorton-test)

add_executable(libmorton-test-bmi2
  "libmorton_test.cpp"
  )
target_compile_options(libmorton-test-bmi2
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: # GCC and clang
      -march=haswell -Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>: # MSVC
      /arch:AVX2 /W3>
  )
target_link_libraries(libmorton-test-bmi2
  PRIVATE
    libmorton::libmorton
  )
add_test(libmorton-test-bmi2 libmorton-test-bmi2)

add_executable(libmorton-test-avx512
  "libmorton_test.cpp"
  )
target_compile_options(libmorton-test-avx512
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: # GCC and clang
      -march=icelake-client -Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>: # MSVC
      /arch:AVX512 /W3>
  )
target_link_libraries(libmorton-test-avx512
  PRIVATE
    libmorton::libmorton
  )
#add_test(libmorton-test-avx512 libmorton-test-avx512)

add_executable(libmorton-test-zen2
  "libmorton_test.cpp"
  )
target_compile_options(libmorton-test-zen2
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: # GCC and clang
      -march=znver2 -Wall -Wextra -Wpedantic>
  )
target_link_libraries(libmorton-test-zen2
  PRIVATE
    libmorton::libmorton
  )
#add_test(libmorton-test-zen2 libmorton-test-zen2)