cmake_minimum_required(VERSION 3.16)

project(frontend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")

set(LLVM_REQUIRED_COMPONENTS core support)
llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_REQUIRED_COMPONENTS})

message(STATUS "LLVM mapped libs: ${LLVM_LIBS}")

add_library(frontend_headers INTERFACE)
target_include_directories(frontend_headers
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
    ${LLVM_INCLUDE_DIRS}
)
target_compile_definitions(frontend_headers INTERFACE ${LLVM_DEFINITIONS})
add_library(frontend::headers ALIAS frontend_headers)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

BISON_TARGET(Parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp
)

FLEX_TARGET(Lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

add_custom_target(generate_parser
    DEPENDS ${BISON_Parser_OUTPUTS}
)

add_executable(frontend
    src/main.cpp
    src/driver.cpp
    src/expr_evaluator.cpp
    src/simulator.cpp
    src/graph_dump.cpp
    src/codegen/codegen.cpp
    ${FLEX_Lexer_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)
option(GRAPH_DUMP "Enable Graphviz dump" OFF)

if (GRAPH_DUMP)
    target_compile_definitions(frontend PRIVATE GRAPH_DUMP)
endif()

if (INTERPRET)
    target_compile_definitions(frontend PRIVATE INTERPRET)
endif()

target_include_directories(frontend PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/graph_dump
    ${CMAKE_CURRENT_SOURCE_DIR}/include/data_structures
    ${CMAKE_CURRENT_SOURCE_DIR}/include/parser
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_definitions(frontend PRIVATE
    ${LLVM_DEFINITIONS}
)

target_link_libraries(frontend PRIVATE ${LLVM_LIBS})

add_subdirectory(tests)
