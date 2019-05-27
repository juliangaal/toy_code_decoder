cmake_minimum_required(VERSION 3.1)
project(ToyDecoder VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(AUTO_TEST "Enables Tests Post build" OFF)

find_package(OpenCV 4.0 REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(Catch2 REQUIRED)

enable_testing()

set(SOURCES src/toy_decoder.cpp src/util.cpp)
set(HEADERS include/toy_decoder/toy_decoder.hpp include/toy_decoder/util.hpp)
add_library(toy_decoder ${SOURCES} ${HEADERS})
target_include_directories(toy_decoder PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)
target_compile_options(toy_decoder PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
target_link_libraries(toy_decoder PUBLIC
                      ${OpenCV_LIBS}
                      fmt::fmt)

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include)
target_compile_options(main PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
target_link_libraries(main PRIVATE
                      ${OpenCV_LIBS}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY}
                      fmt::fmt
                      toy_decoder)

add_executable(unit_test tests/test.cpp)
add_test(NAME unit_test COMMAND unit_test)
target_include_directories(unit_test PRIVATE include)
target_compile_options(unit_test PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(unit_test PRIVATE
                      ${OpenCV_LIBS}
                      fmt::fmt
                      Catch2::Catch2
                      toy_decoder)

message(STATUS "AUTO_TEST is ${AUTO_TEST}")
if (AUTO_TEST)
    add_custom_command(
            TARGET unit_test
            COMMENT "Run tests"
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    )
endif ()

install(TARGETS toy_decoder
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ToyDecoder::
        DESTINATION lib/cmake/${PROJECT_NAME})

install(FILES ${HEADERS} DESTINATION include/toy_decoder)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY SameMajorVersion)

install(FILES "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})

