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
      ${CMAKE_SOURCE_DIR}/src/clrc/state_machine.c
      ${CMAKE_SOURCE_DIR}/src/clrc/token.cpp
  )
  target_link_libraries(${TEST_EXE}
    PUBLIC
      ${CONAN_LIBS}
  )
  add_test(NAME ${TEST_EXE} COMMAND ${TEST_EXE})
endfunction()

add_claire_test(lexer test_lexer.cpp)
