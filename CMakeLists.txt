cmake_minimum_required(VERSION 3.1)
project(hwcpp)

set(CMAKE_CXX_STANDARD 14)
set(AUTO_TEST OFF CACHE BOOL "Enables Tests Post build")

find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Catch2 REQUIRED)

enable_testing()

set(EXE blob)
add_executable(${EXE} src/blob.cpp)
target_include_directories(${EXE} PRIVATE include)
add_compile_options(-Wall -Wextra -pedantic -Werror)
target_link_libraries(${EXE}
        ${OpenCV_LIBS}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        fmt::fmt)

set(UNIT_TEST test_rot)
add_executable(${UNIT_TEST} tests/test.cpp)
add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})
target_include_directories(${UNIT_TEST} PRIVATE include)
add_compile_options(-Wall -Wextra -pedantic -Werror)
target_link_libraries(${UNIT_TEST}
        ${OpenCV_LIBS}
        fmt::fmt
        Catch2::Catch2)

message(STATUS "AUTO_TEST is ${AUTO_TEST}")
if(AUTO_TEST) 
  add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose
)
endif()
