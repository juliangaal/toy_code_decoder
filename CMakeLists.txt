cmake_minimum_required(VERSION 3.1)
project(NotQRCode VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV 4.0 REQUIRED)
find_package(Git REQUIRED)

option(COMPILE_TEST "Compiles Tests" OFF)
option(ENABLE_AUTO_TEST "Enables Tests Post build" OFF)
option(GIT_SUBMODULE "Check submodules during build" ON)
option(PY_BINDINGS "Enable Python bindings generation" OFF)

set(LIB notqrcode)
set(PY_LIB notqrcode_py)
set(PY_GEN_PATH ${PROJECT_SOURCE_DIR}/scripts CACHE STRING "Where generated library will be placed")

# notqrcode library
set(SOURCES src/notqrcode_decoder.cpp src/util.cpp)
set(HEADERS include/notqrcode/notqrcode_decoder.hpp include/notqrcode/util.hpp)

add_library(${LIB} STATIC ${SOURCES} ${HEADERS})
target_compile_options(${LIB} PRIVATE -Wall -Weffc++ -Wpedantic -Werror -ffast-math -Wextra)
target_include_directories(${LIB} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)
target_link_libraries(${LIB} PUBLIC ${OpenCV_LIBS})

function(update_submodule package_name)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${package_name}/CMakeLists.txt")
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${package_name}/CMakeLists.txt")
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    endif()
endfunction(update_submodule)

message(STATUS "PY_BINDINGS set to ${PY_BINDINGS}")
if(PY_BINDINGS)
    update_submodule(pybind11)

    # pybind11 bindings
    add_subdirectory(pybind11)
    pybind11_add_module(${PY_LIB} bindings/bindings.cpp)
    target_link_libraries(${PY_LIB} PRIVATE ${LIB})
    set_target_properties(${PY_LIB}
                          PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY ${PY_GEN_PATH})
endif()

# enable tests compilation when COMPILE_TEST=ON
message(STATUS "COMPILE_TEST set to ${COMPILE_TEST}")
if(COMPILE_TEST)
    find_package(fmt REQUIRED)
    update_submodule(doctest)
    add_subdirectory(doctest)
    enable_testing()

    add_executable(unit_test tests/test.cpp)
    add_test(NAME unit_test COMMAND unit_test)
    target_include_directories(unit_test PRIVATE include)
    target_compile_options(unit_test PRIVATE -Wall -Wextra -pedantic -Werror)
    target_link_libraries(unit_test PRIVATE
                          ${OpenCV_LIBS}
                          doctest
                          notqrcode)

    add_executable(main tests/main.cpp)
    target_include_directories(main PRIVATE include)
    target_compile_options(main PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(main PRIVATE
                          ${OpenCV_LIBS}
                          fmt::fmt
                          notqrcode)

    add_executable(video tests/video.cpp)
    add_test(NAME video COMMAND video)
    target_include_directories(video PRIVATE include)
    target_compile_options(video PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(video PRIVATE
            ${OpenCV_LIBS}
            fmt::fmt
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

