cmake_minimum_required(VERSION 3.5)
project(hwcpp)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost COMPONENTS filesystem system REQUIRED)

enable_testing()

add_executable(blob src/blob.cpp)
target_link_libraries(blob 
    ${OpenCV_LIBS} 
    ${Boost_SYSTEM_LIBRARY} 
    ${Boost_FILESYSTEM_LIBRARY} 
    fmt::fmt)

add_executable(test_rot test/test.cpp)
target_link_libraries(blob
        ${OpenCV_LIBS}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        fmt::fmt)

set(UNIT_TEST test_rot)
add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "Run tests"
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failures
)
