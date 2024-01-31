#include "../include/model.h"

libusb_device **device_list;
ssize_t device_count;
Node *blocked_list = NULL;
static char *usb_filepath = "/home/mawerid/system_prog/lab7/devices.json";

libusb_context *context = NULL;
libusb_hotplug_callback_handle *callback_handle;

extern pthread_t thread_model;
extern pthread_t thread_view;

// list methods
Node *create_node(gboolean connected,
                  uint16_t id_vendor,
                  uint16_t id_product) {
    Node *node;
    node = malloc(sizeof(Node));
    if (node == NULL)
        return NULL;

    node->prev = NULL;
    node->connected = connected;
    node->id_vendor = id_vendor;
    node->id_product = id_product;
    node->next = NULL;

    return node;
}

ExitCode delete_node(Node *node) {
    if (node == NULL)
        return NO_SUCH_POINTER;

    free(node);

    return SUCCESS;

}

ExitCode delete_list(Node *list) {
    if (list == NULL)
        return NO_SUCH_POINTER;

    Node *current = list->next;
    while (current) {
        Node *next = current->next;
        delete_node(current);
        current = next;
    }

    free(list);
    return SUCCESS;
}

ExitCode append(Node *list,
                gboolean connected,
                uint16_t id_vendor,
                uint16_t id_product) {
    if (list == NULL)
        return NO_SUCH_POINTER;

    Node *new_node;
    new_node = create_node(connected, id_vendor, id_product);
    if (new_node == NULL)
        return MEMORY_ALLOCATION_FAILURE;

    Node *ptr;
    ptr = list;
    for (; ptr->next != NULL; ptr = ptr->next);

    ptr->next = new_node;
    new_node->prev = ptr;

    return SUCCESS;
}

ExitCode delete(Node *list,
                uint16_t id_vendor,
                uint16_t id_product) {
    if (list == NULL)
        return NO_SUCH_POINTER;

    Node *ptr;
    ptr = search(list, id_vendor, id_product);
    if (ptr == NULL)
        return NO_SUCH_NODE;

    if (ptr->prev != NULL)
        ptr->prev->next = ptr->next;

    if (ptr->next != NULL)
        ptr->next->prev = ptr->prev;

    delete_node(ptr);

    return SUCCESS;
}

Node *search(Node *list,
             uint16_t id_vendor,
             uint16_t id_product) {
    if (list == NULL)
        return NULL;

    Node *ptr;
    ptr = list;
    for (; ptr != NULL; ptr = ptr->next)
        if (ptr->id_vendor == id_vendor && ptr->id_product == id_product)
            return ptr;

    return NULL;
}

// Main function to run
void *control(void *argc) {
    usb_filepath = (char *) argc;

    int ret_val;

    if (libusb_init(&context) < 0)
        error_handler(LIBUSB_ERROR);

    // Get the list of USB devices
    device_count = libusb_get_device_list(context, &device_list);
    if (device_count < 0) {
        libusb_exit(context);
        error_handler(LIBUSB_ERROR);
    }
    save_devices_to_file(usb_list_to_node_list(device_list, device_count),
                         usb_filepath);

    if (load_config(blocked_list)) {
        printf("No config file founded\n");
    }

    ret_val = libusb_hotplug_register_callback(context,
                                               LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED
                                                   | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                               0,
                                               LIBUSB_HOTPLUG_MATCH_ANY,
                                               LIBUSB_HOTPLUG_MATCH_ANY,
                                               LIBUSB_HOTPLUG_MATCH_ANY,
                                               (libusb_hotplug_callback_fn) hotplug_callback,
                                               blocked_list,
                                               callback_handle);

    if (ret_val != LIBUSB_SUCCESS) {
        fprintf(stderr, "Error registering hotplug callback\n");
        delete_list(blocked_list);
        libusb_free_device_list(device_list, 1);
        libusb_exit(context);
        error_handler(LIBUSB_ERROR);
    }

    signal(SIGINT | SIGUSR1, signal_handler);

//    printf("SIGNAL FROM MODEL %lu\n", pthread_self());
//    pthread_kill(thread_model, SIGUSR1);
//    send_to_view(blocked_list, device_list, device_count);

    printf("Vendor ID\tProduct ID\n");

    while (is_running) {
        libusb_handle_events(context);
    }

    printf("Exit...\n");

    save_config(blocked_list);
    save_devices_to_file(usb_list_to_node_list(device_list, device_count),
                         usb_filepath);

    // Free the list and exit
    if (callback_handle)
        libusb_hotplug_deregister_callback(context, *callback_handle);
    if (device_list)
        libusb_free_device_list(device_list, 1);
    delete_list(blocked_list);
    if (context)
        libusb_exit(context);

    pthread_exit(NULL);
}

libusb_device *search_device(libusb_device **p_device_list,
                             ssize_t count,
                             uint16_t id_vendor,
                             uint16_t id_product) {
    libusb_device *device = NULL;

    for (ssize_t i = 0; i < count; i++) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(p_device_list[i], &desc) == 0) {
            if (desc.idVendor == id_vendor && desc.idProduct == id_product) {
                device = p_device_list[i];
                break;
            }
        }
    }

    return device;
}

// handler for interruptions
void print_usb_device_info(libusb_device *dev) {
    struct libusb_device_descriptor desc;
    if (libusb_get_device_descriptor(dev, &desc) == 0) {
        printf("0x%04x\t\t0x%04x\n", desc.idVendor, desc.idProduct);
    }
}

void hotplug_callback(libusb_context *context,
                      libusb_device *dev,
                      libusb_hotplug_event event,
                      Node *p_blocked_list) {

    struct libusb_device_descriptor desc;
    if (libusb_get_device_descriptor(dev, &desc) == 0) {
        Node *node = search(p_blocked_list, desc.idVendor, desc.idProduct);
        if (node && event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
            node->connected = TRUE;
            printf("Blocked device connected:\n");
            print_usb_device_info(dev);
        } else if (node && event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
            node->connected = FALSE;
            printf("Blocked device disconnected:\n");
            print_usb_device_info(dev);
        } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
            printf("Device connected:\n");
            print_usb_device_info(dev);
        } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
            printf("Device disconnected:\n");
            print_usb_device_info(dev);
        }
        signal_handler(SIGUSR1);
    }
}

// error handler
void error_handler(ExitCode error) {
    switch (error) {
        case SUCCESS:break;
        case FILE_NOT_FOUND:fprintf(stderr, "Error: File not found\n");
            break;
        case MEMORY_ALLOCATION_FAILURE:
            fprintf(stderr,
                    "Error: Memory allocation failure\n");
            break;
        case NO_SUCH_POINTER:fprintf(stderr, "Error: No such pointer\n");
            break;
        case NO_CONFIG_FILE:fprintf(stderr, "Error: No configuration file\n");
            break;
        case NO_SUCH_NODE:fprintf(stderr, "Error: No such node\n");
            break;
        case NO_SUCH_DEVICE:fprintf(stderr, "Error: No such device\n");
            break;
        case CONFIG_READ_ERROR:
            fprintf(stderr,
                    "Error: Configuration read error\n");
            break;
        case JSON_CONVERSION_ERROR:
            fprintf(stderr,
                    "Error: JSON conversion error\n");
            break;
        case LIBUSB_ERROR:fprintf(stderr, "Error: libusb error\n");
            break;
        default:fprintf(stderr, "Error: Unknown exit code\n");
            break;
    }
}

// communicate with gui
Node *usb_list_to_node_list(libusb_device **p_device_list,
                            ssize_t p_device_count) {
    Node *list = NULL;
    struct libusb_device_descriptor desc;
    int ret_val;

    if (p_device_list == NULL)
        printf("Nope\n");

    for (int i = 0; i < p_device_count; i++) {
        ret_val = libusb_get_device_descriptor(p_device_list[i], &desc);
        if (ret_val < 0) {
            fprintf(stderr,
                    "libusb_get_device_descriptor error: %s\n",
                    libusb_error_name(ret_val));
            continue;
        }
        if (list == NULL)
            list = create_node(TRUE, desc.idVendor, desc.idProduct);
        else
            append(list, TRUE, desc.idVendor, desc.idProduct);
    }

    return list;
}

ExitCode send_to_view(Node *p_blocked_list,
                      libusb_device **p_device_list,
                      ssize_t p_device_count) {
    printf("Send to view...\n");
    Node *dev_list = usb_list_to_node_list(p_device_list, p_device_count);

    printf("SIGNAL FROM MODEL %lu\n", pthread_self());
    pthread_kill(thread_view, SIGUSR2);

    char buffer[64];

    printf("MODEL: %zd\n", p_device_count);
    sprintf(buffer, "%zd", p_device_count);

    int named_pipe = -1;
    while (named_pipe < 0)
        named_pipe = open(pipe_name, O_WRONLY | O_NONBLOCK);

    pthread_mutex_lock(&threadData.mutex);
    write(named_pipe, buffer, sizeof(buffer));
    pthread_mutex_unlock(&threadData.mutex);
    for (Node *ptr = dev_list; ptr != NULL; ptr = ptr->next) {

        gboolean is_blocked = FALSE;
        if (search(p_blocked_list, ptr->id_vendor, ptr->id_product))
            is_blocked = TRUE;

        printf("MODEL: %hu %hu %d\n",
               ptr->id_vendor,
               ptr->id_product,
               is_blocked);

        sprintf(buffer,
                "%hu %hu %d",
                ptr->id_vendor,
                ptr->id_product,
                is_blocked);

        pthread_mutex_lock(&threadData.mutex);
        write(named_pipe, buffer, sizeof(buffer));
        pthread_mutex_unlock(&threadData.mutex);
    }

    delete_list(dev_list);

    close(named_pipe);

    return SUCCESS;
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        is_running = 0;
    } else if (signal == SIGUSR1) {
        if (pthread_self() == thread_model) {
            if (threadData.productID != 0 && threadData.vendorID != 0) {
                libusb_device *device = search_device(device_list,
                                                      device_count,
                                                      threadData.vendorID,
                                                      threadData.productID);
                if (search(blocked_list,
                           threadData.vendorID,
                           threadData.productID))
                    unblock_device(blocked_list,
                                   device_list,
                                   device_count,
                                   device);
                else
                    block_device(blocked_list,
                                 device_list,
                                 device_count,
                                 device);

                update_blocked_list(blocked_list, device_list, device_count);
                save_config(blocked_list);

                send_to_view(blocked_list, device_list, device_count);

                pthread_mutex_lock(&threadData.mutex);
                threadData.productID = 0;
                threadData.vendorID = 0;
                pthread_mutex_unlock(&threadData.mutex);
            } else {
                libusb_free_device_list(device_list, 1);
                device_count = libusb_get_device_list(context, &device_list);
                if (device_count < 0) {
                    libusb_exit(context);
                    error_handler(LIBUSB_ERROR);
                }
                save_devices_to_file(usb_list_to_node_list(device_list,
                                                           device_count),
                                     usb_filepath);
                update_blocked_list(blocked_list, device_list, device_count);

                send_to_view(blocked_list, device_list, device_count);

            }
        }
    }
}

// work with configs
ExitCode load_config(Node *list) {
    return load_devices_from_file(list, config_path);
}

ExitCode save_config(Node *list) {
    return save_devices_to_file(list, config_path);
}

// work with file
ExitCode load_devices_from_file(Node *list, const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file)
        return NO_CONFIG_FILE;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_content = (char *) malloc(file_size + 1);
    if (!file_content) {
        fclose(file);
        return MEMORY_ALLOCATION_FAILURE;
    }

    fread(file_content, 1, file_size, file);
    fclose(file);

    file_content[file_size] = '\0';

    cJSON *config;
    config = cJSON_Parse(file_content);

    free(file_content);

    if (!config) {
        fprintf(stderr, "Error parsing JSON in file: %s\n", filepath);
        return CONFIG_READ_ERROR;
    }

    delete_list(list);
    list = json_to_list(config);
    cJSON_Delete(config);

    return SUCCESS;
}

ExitCode save_devices_to_file(Node *list, const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (!file)
        return NO_CONFIG_FILE;

    cJSON *config = list_to_json(list);

    char *json_str = cJSON_Print(config);
    if (!json_str) {
        fclose(file);
        return JSON_CONVERSION_ERROR;
    }

    fprintf(file, "%s", json_str);

    fclose(file);
    free(json_str);

    cJSON_Delete(config);

    return SUCCESS;
}

// convert JSON and List of device
cJSON *device_to_json(gboolean connected,
                      uint16_t id_vendor,
                      uint16_t id_product) {
    cJSON *json = cJSON_CreateObject();

    cJSON_AddBoolToObject(json, "connected", connected);
    cJSON_AddNumberToObject(json, "id_vendor", id_vendor);
    cJSON_AddNumberToObject(json, "id_product", id_product);

    return json;
}

Node *json_to_device(cJSON *config) {
    Node *device;
    device = malloc(sizeof(Node));
    if (!device)
        return NULL;

    cJSON
        *connected_json = cJSON_GetObjectItemCaseSensitive(config, "connected");
    cJSON
        *id_vendor_json = cJSON_GetObjectItemCaseSensitive(config, "id_vendor");
    cJSON *id_product_json =
        cJSON_GetObjectItemCaseSensitive(config, "id_product");

    if (cJSON_IsBool(connected_json) && cJSON_IsNumber(id_vendor_json)
        && cJSON_IsNumber(id_product_json)) {
        device->connected = cJSON_IsTrue(connected_json);
        device->id_vendor = id_vendor_json->valueint;
        device->id_product = id_product_json->valueint;
    } else {
        free(device);
        return NULL;
    }

    return device;
}

Node *json_to_list(cJSON *config) {
    if (!cJSON_IsArray(config))
        return NULL;

    int num_devices = cJSON_GetArraySize(config);
    Node *devices = NULL;

    for (int i = 0; i < num_devices; i++) {
        cJSON *device_json = cJSON_GetArrayItem(config, i);
        Node *device = json_to_device(device_json);
        if (devices == NULL)
            devices = create_node(device->connected,
                                  device->id_vendor,
                                  device->id_product);
        else
            append(devices,
                   device->connected,
                   device->id_vendor,
                   device->id_product);
        free(device);
    }

    return devices;
}

cJSON *list_to_json(Node *list) {
    cJSON *json_array = cJSON_CreateArray();

    Node *ptr;
    ptr = list;
    for (; ptr != NULL; ptr = ptr->next) {
        cJSON *device_json =
            device_to_json(ptr->connected, ptr->id_vendor, ptr->id_product);
        cJSON_AddItemToArray(json_array, device_json);
    }

    return json_array;
}

// real-time update status
ExitCode update_blocked_list(Node *list,
                             libusb_device **p_device_list,
                             ssize_t p_device_count) {

    struct libusb_device_descriptor desc;
    int ret_val;

    for (ssize_t i = 0; i < p_device_count; i++) {
        ret_val = libusb_get_device_descriptor(p_device_list[i], &desc);
        if (ret_val < 0) {
            fprintf(stderr,
                    "libusb_get_device_descriptor error: %s\n",
                    libusb_error_name(ret_val));
            continue;
        }

        Node *device = search(list, desc.idVendor, desc.idProduct);

        if (device != NULL && device->connected == FALSE)
            device->connected = TRUE;
    }

    return SUCCESS;
}

// block/unblock
ExitCode block_device(Node *list,
                      libusb_device **p_device_list,
                      ssize_t p_device_count,
                      libusb_device *device) {
    gint ret;
    libusb_device_handle *dev_handle = NULL;
    struct libusb_device_descriptor desc;

    ret = libusb_get_device_descriptor(device, &desc);
    if (ret < 0)
        return NO_SUCH_DEVICE;

    ret = libusb_open(device, &dev_handle);

    if (ret != LIBUSB_SUCCESS)
        return LIBUSB_ERROR;

    // Block the USB device
    ret = libusb_kernel_driver_active(dev_handle, 0);
    if (ret == 1) {
        ret = libusb_detach_kernel_driver(dev_handle, 0);
        if (ret != LIBUSB_SUCCESS)
            return LIBUSB_ERROR;
    } else if (ret != 0)
        return LIBUSB_ERROR;

    if (dev_handle != NULL)
        libusb_close(dev_handle);

    delete(list, desc.idVendor, desc.idProduct);

    send_to_view(list, p_device_list, p_device_count);

    return SUCCESS;
}

ExitCode unblock_device(Node *list,
                        libusb_device **p_device_list,
                        ssize_t p_device_count,
                        libusb_device *device) {
    gint ret;
    libusb_device_handle *dev_handle = NULL;
    struct libusb_device_descriptor desc;

    ret = libusb_get_device_descriptor(device, &desc);
    if (ret < 0)
        return NO_SUCH_DEVICE;

    ret = libusb_open(device, &dev_handle);

    if (ret != LIBUSB_SUCCESS)
        return LIBUSB_ERROR;

    // Unblock the USB device
    ret = libusb_kernel_driver_active(dev_handle, 0);
    if (ret == 0) {
        ret = libusb_attach_kernel_driver(dev_handle, 0);
        if (ret != LIBUSB_SUCCESS)
            return LIBUSB_ERROR;
    } else if (ret != 1)
        return LIBUSB_ERROR;

    if (dev_handle != NULL)
        libusb_close(dev_handle);

    if (list == NULL)
        list = create_node(TRUE, desc.idVendor, desc.idProduct);
    else
        append(list, TRUE, desc.idVendor, desc.idProduct);

    send_to_view(list, p_device_list, p_device_count);

    return SUCCESS;
}
