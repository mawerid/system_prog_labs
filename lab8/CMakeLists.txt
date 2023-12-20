cmake_minimum_required(VERSION 3.26)
project(file_control C)

set(CMAKE_C_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-2.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

add_subdirectory(src)                              
add_subdirectory(app)  