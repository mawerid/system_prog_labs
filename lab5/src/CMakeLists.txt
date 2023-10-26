set(HEADER_TASKS "${lab5_SOURCE_DIR}/include/tasks.h")

add_library(tasks tasks.c ${HEADER_TASKS})

target_include_directories(tasks PUBLIC ../include)
