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
    exception.cpp
    codegen/ir_code_generator.cpp
    parser/ast.cpp
    parser/parser.cpp
    parser/lexer.cpp
    parser/state_machine.c
    parser/token.cpp
)
target_link_libraries(${TARGET}
  PUBLIC
    ${CONAN_LIBS}
)
