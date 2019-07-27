# enable tests compilation when COMPILE_EXAMPLES=ON
message(STATUS "COMPILE_EXAMPLES set to ${COMPILE_EXAMPLES}")
if(COMPILE_EXAMPLES)
    find_package(fmt REQUIRED)
    add_executable(video video.cpp)
    target_include_directories(video PRIVATE include)
    target_compile_options(video PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(video PRIVATE ${OpenCV_LIBS} fmt::fmt notqrcode)

    add_executable(video_with_params video_with_params.cpp)
    target_include_directories(video_with_params PRIVATE include)
    target_compile_options(video_with_params PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(video_with_params PRIVATE ${OpenCV_LIBS} fmt::fmt notqrcode)

    add_executable(file file.cpp)
    target_include_directories(file PRIVATE include)
    target_compile_options(file PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(file PRIVATE ${OpenCV_LIBS} fmt::fmt notqrcode)

    add_executable(file_with_params file_with_params.cpp)
    target_include_directories(file_with_params PRIVATE include)
    target_compile_options(file_with_params PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(file_with_params PRIVATE ${OpenCV_LIBS} fmt::fmt notqrcode)

    add_executable(img img.cpp)
    target_include_directories(img PRIVATE include)
    target_compile_options(img PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(img PRIVATE ${OpenCV_LIBS} fmt::fmt notqrcode)

    add_executable(img_with_params img_with_params.cpp)
    target_include_directories(img_with_params PRIVATE include)
    target_compile_options(img_with_params PRIVATE -Wall -Wextra -Weffc++ -pedantic -Werror)
    target_link_libraries(img_with_params PRIVATE ${OpenCV_LIBS} fmt::fmt notqrcode)
endif()