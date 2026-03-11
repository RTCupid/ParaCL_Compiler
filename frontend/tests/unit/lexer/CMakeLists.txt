find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
include(GoogleTest)

set(SRC_LIST
    src/process.cpp
    src/get_line_and_column.cpp
    ${FLEX_Lexer_OUTPUTS}
)

set_source_files_properties(
    ${FLEX_Lexer_OUTPUTS}
    PROPERTIES GENERATED TRUE
)

add_executable(lexer ${SRC_LIST})
add_dependencies(lexer generate_parser)

target_include_directories(lexer PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(lexer PRIVATE ${LLVM_DEFINITIONS})

target_link_libraries(lexer
    PRIVATE
    frontend::headers
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
    ${LLVM_LIBS}  
)

gtest_discover_tests(lexer PROPERTIES LABELS "unit")
