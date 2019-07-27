# enable tests compilation when COMPILE_TEST=ON
message(STATUS "COMPILE_TESTS set to ${COMPILE_TESTS}")
if(COMPILE_TESTS)
    find_package(Catch2 REQUIRED)
    find_package(fmt REQUIRED)
    enable_testing()

    add_executable(unit_test unit_test.cpp)
    add_test(NAME unit_test COMMAND unit_test)
    target_include_directories(unit_test PRIVATE include)
    target_compile_options(unit_test PRIVATE -Wall -Wextra -pedantic -Werror)
    target_link_libraries(unit_test PRIVATE ${OpenCV_LIBS} Catch2::Catch2 notqrcode fmt::fmt)

    add_executable(integration_test integration_test.cpp)
    add_test(NAME integration_test COMMAND integration_test)
    target_include_directories(integration_test PRIVATE include)
    target_compile_options(integration_test PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(integration_test PRIVATE ${OpenCV_LIBS} Catch2::Catch2 notqrcode fmt::fmt)
endif()