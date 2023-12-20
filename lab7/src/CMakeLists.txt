set(HEADER_VIEW "${usb_control_SOURCE_DIR}/include/view.h")
set(HEADER_MODEL "${usb_control_SOURCE_DIR}/include/model.h")

add_library(view view.c ${HEADER_VIEW})
add_library(model model.c ${HEADER_MODEL})

target_include_directories(view PUBLIC ../include)
target_include_directories(model PUBLIC ../include)
