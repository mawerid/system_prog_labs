cmake_minimum_required(VERSION 3.26)
project(file_control C)

set(CMAKE_C_STANDARD 17)

add_executable(file_control main.c)

# target_link_libraries(file_control view model viewmodel ${GTK_LIBRARIES})