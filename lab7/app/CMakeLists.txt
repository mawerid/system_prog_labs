add_executable(usb_control main.c)

target_link_libraries(usb_control PkgConfig::GTK view model libusb-1.0.a udev cjson)