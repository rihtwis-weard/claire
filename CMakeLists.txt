cmake_minimum_required(VERSION 3.16.3)
project(claire
  VERSION 0.1.0
  DESCRIPTION "Simple and elegant functional programming language"
  HOMEPAGE_URL "https://github.com/rihtwis-weard/claire"
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CLAIRE_COMPILE_OPTIONS
  -Wall
  -Wextra
  -Werror
  -Wno-unused
  -pedantic-errors
  $<$<COMPILE_LANGUAGE:CXX>:
    -fconcepts
  >
)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
