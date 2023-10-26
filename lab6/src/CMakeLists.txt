set(HEADER_TASK_1 "${file_util_SOURCE_DIR}/include/task1.h")
set(HEADER_TASK_2 "${file_util_SOURCE_DIR}/include/task2.h")

add_library(task1 task1.c ${HEADER_TASK_1})
add_library(task2 task2.c ${HEADER_TASK_2})

target_include_directories(task1 PUBLIC ../include)
target_include_directories(task2 PUBLIC ../include)
