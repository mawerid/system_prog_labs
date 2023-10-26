set(HEADER_TASKS "${file_util_SOURCE_DIR}/include/tasks.h")

add_library(tasks tasks.c ${HEADER_TASK_1})

target_include_directories(tasks PUBLIC ../include)
