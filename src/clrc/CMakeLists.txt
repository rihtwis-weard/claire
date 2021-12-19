project(clrc
  VERSION 0.1.0
  DESCRIPTION "Claire compiler"
)

set(TARGET clrc)

add_executable(${TARGET} main.cpp)
target_compile_options(${TARGET}
  PRIVATE
    ${CLAIRE_TARGET_COMPILE_OPTIONS}
)
target_sources(${TARGET}
  PRIVATE
)
target_link_libraries(${TARGET}
  PUBLIC
    ${CONAN_LIBS}
)
