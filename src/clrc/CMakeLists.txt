project(clrc
  VERSION 0.1.0
  DESCRIPTION "Claire compiler"
)

set(TARGET clrc)

add_executable(${TARGET} main.cpp)
target_compile_options(${TARGET}
  PRIVATE
    ${CLAIRE_COMPILE_OPTIONS}
)
target_sources(${TARGET}
  PRIVATE
    codegen.cpp
    exception.cpp
    lexer.cpp
    parser.cpp
    state_machine.c
    token.cpp
)
target_link_libraries(${TARGET}
  PUBLIC
    ${CONAN_LIBS}
)
