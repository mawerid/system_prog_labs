#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <glib.h>
#include <libusb-1.0/libusb.h>
#include <cjson/cJSON.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

static uint8_t is_running = 1;

static char *pipe_name = "usb_control_fifo";

static libusb_context *context = NULL;
static libusb_hotplug_callback_handle *callback_handle;

// Data structure to share between threads
struct ThreadData {
    uint16_t productID;
    uint16_t vendorID;
    pthread_mutex_t mutex;
};

// Global data structure
static struct ThreadData threadData;
static pthread_t thread_model;
static pthread_t thread_view;

static const char *config_path = "../../data/config.json";

typedef enum {
    SUCCESS = 0,
    FILE_NOT_FOUND,
    MEMORY_ALLOCATION_FAILURE,
    NO_SUCH_POINTER,
    NO_CONFIG_FILE,
    NO_SUCH_NODE,
    NO_SUCH_DEVICE,
    CONFIG_READ_ERROR,
    JSON_CONVERSION_ERROR,
    LIBUSB_ERROR
} ExitCode;

typedef struct Node {
    gboolean connected;
    uint16_t id_vendor;
    uint16_t id_product;
    struct Node *prev;
    struct Node *next;
} Node;

// list methods
Node *create_node(gboolean connected,
                  uint16_t id_vendor,
                  uint16_t id_product);
ExitCode delete_node(Node *node);
ExitCode delete_list(Node *list);
ExitCode append(Node *list,
                gboolean connected,
                uint16_t id_vendor,
                uint16_t id_product);
ExitCode delete(Node *list,
                uint16_t id_vendor,
                uint16_t id_product);
Node *search(Node *list,
             uint16_t id_vendor,
             uint16_t id_product);

// Main function to run
void *control(void *argc);

// handler for interruptions
libusb_device *search_device(libusb_device **p_device_list,
                             ssize_t count,
                             uint16_t id_vendor,
                             uint16_t id_product);
void print_usb_device_info(libusb_device *dev);
void hotplug_callback(libusb_context *context,
                      libusb_device *dev,
                      libusb_hotplug_event event,
                      Node *p_blocked_list);

// error handler
void error_handler(ExitCode error);

// communicate with gui
Node *usb_list_to_node_list(libusb_device **p_device_list,
                            ssize_t p_device_count);
void signal_handler(int signal);
ExitCode send_to_view(Node *p_blocked_list,
                      libusb_device **p_device_list,
                      ssize_t p_device_count);

// work with json configs
ExitCode load_config(Node *list);
ExitCode save_config(Node *list);

// work with json file
ExitCode load_devices_from_file(Node *list, const char *filepath);
ExitCode save_devices_to_file(Node *list, const char *filepath);

// convert JSON and List of device
cJSON *list_to_json(Node *list);
Node *json_to_list(cJSON *config);
cJSON *device_to_json(gboolean connected,
                      uint16_t id_vendor,
                      uint16_t id_product);
Node *json_to_device(cJSON *config);

// real-time update status
ExitCode update_blocked_list(Node *list,
                             libusb_device **p_device_list,
                             ssize_t p_device_count);

// block/unblock
ExitCode block_device(Node *list,
                      libusb_device **p_device_list,
                      ssize_t p_device_count,
                      libusb_device *device);
ExitCode unblock_device(Node *list,
                        libusb_device **p_device_list,
                        ssize_t p_device_count,
                        libusb_device *device);

#endif
