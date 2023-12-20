set(HEADER_VIEW "${file_control_SOURCE_DIR}/include/view.h")
set(HEADER_MODEL "${file_control_SOURCE_DIR}/include/model.h")
set(HEADER_VIEWMODEL "${file_control_SOURCE_DIR}/include/viewmodel.h")


add_library(view view.c ${HEADER_VIEW})
add_library(model model.c ${HEADER_MODEL})
add_library(viewmodel viewmodel.c ${HEADER_VIEWMODEL})

target_include_directories(view PUBLIC ../include)
target_include_directories(model PUBLIC ../include)
target_include_directories(viewmodel PUBLIC ../include)
