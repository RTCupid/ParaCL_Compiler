cmake_minimum_required(VERSION 3.16)

project(language LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_custom_target(check_program_termination
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/frontend/tests/end_to_end/check_program_termination.sh)

add_test(
    NAME check_program_termination_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/frontend/tests/end_to_end/check_program_termination.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

enable_testing()
add_subdirectory(frontend)
