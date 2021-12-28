function(add_claire_test TARGET ENTRY)
  set(TEST_EXE test_${TARGET})

  add_executable(${TEST_EXE} ${ENTRY})
  target_include_directories(${TEST_EXE}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/tests
      ${CMAKE_SOURCE_DIR}/src/clrc
  )
  target_compile_options(${TEST_EXE}
    PRIVATE
      ${CLAIRE_COMPILE_OPTIONS}
  )
  target_sources(${TEST_EXE}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/src/clrc/exception.cpp
      ${CMAKE_SOURCE_DIR}/src/clrc/codegen/ir_code_generator.cpp
      ${CMAKE_SOURCE_DIR}/src/clrc/parser/ast.cpp
      ${CMAKE_SOURCE_DIR}/src/clrc/parser/parser.cpp
      ${CMAKE_SOURCE_DIR}/src/clrc/parser/lexer.cpp
      ${CMAKE_SOURCE_DIR}/src/clrc/parser/state_machine.c
      ${CMAKE_SOURCE_DIR}/src/clrc/parser/token.cpp
  )
  target_link_libraries(${TEST_EXE}
    PUBLIC
      ${CONAN_LIBS}
  )
  add_test(NAME ${TEST_EXE} COMMAND ${TEST_EXE})
endfunction()

add_claire_test(ir_codegen test_ir_codegen.cpp)
add_claire_test(lexer test_lexer.cpp)
add_claire_test(parser test_parser.cpp)
