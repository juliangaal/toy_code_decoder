cmake_minimum_required(VERSION 3.1)
project(NotQRCode VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(COMPILE_TEST "Compiles Tests" OFF)
option(ENABLE_AUTO_TEST "Enables Tests Post build" OFF)

find_package(OpenCV 4.0 REQUIRED)
find_package(fmt REQUIRED)

set(SOURCES src/notqrcode_decoder.cpp src/util.cpp)
set(HEADERS include/notqrcode/notqrcode_decoder.hpp include/notqrcode/util.hpp)
add_library(notqrcode ${SOURCES} ${HEADERS})
target_include_directories(notqrcode PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)
target_compile_options(notqrcode PRIVATE -Wall -Wextra -Weffc++ -pedantic -ffast-math -Werror)
target_link_libraries(notqrcode PUBLIC
                      ${OpenCV_LIBS}
                      fmt::fmt)

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include)
target_compile_options(main PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
target_link_libraries(main PRIVATE
                      ${OpenCV_LIBS}
                      fmt::fmt
                      notqrcode)

# enable tests compilation when COMPILE_TEST=ON
message(STATUS "COMPILE_TEST set to ${COMPILE_TEST}")
if(COMPILE_TEST)
    find_package(Catch2 REQUIRED)
    enable_testing()

    add_executable(unit_test tests/test.cpp)
    add_test(NAME unit_test COMMAND unit_test)
    target_include_directories(unit_test PRIVATE include)
    target_compile_options(unit_test PRIVATE -Wall -Wextra -pedantic -Werror)
    target_link_libraries(unit_test PRIVATE
                          ${OpenCV_LIBS}
                          fmt::fmt
                          Catch2::Catch2
                          notqrcode)
endif()

# enable target test when ENABLE_AUTO_TEST=ON
message(STATUS "ENABLE_AUTO_TEST set to ${ENABLE_AUTO_TEST}")
if(ENABLE_AUTO_TEST)
    add_custom_command(
            TARGET unit_test
            COMMENT "Run tests"
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
endif()

# generate docs
add_custom_target(doc)
add_custom_command(TARGET doc
                   COMMAND cd ${PROJECT_SOURCE_DIR}/docs && doxygen Doxyfile && moxygen .)

# install
install(TARGETS notqrcode
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE NotQRCode::
        DESTINATION lib/cmake/${PROJECT_NAME})

install(FILES ${HEADERS} DESTINATION include/notqrcode)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY SameMajorVersion)

install(FILES "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})

