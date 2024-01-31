#pragma once
#ifndef VIEW_H
#define VIEW_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib-unix.h>

#include "model.h"

#define CSS_PATH "../../data/style.css"

// Struct to represent each row in the table
typedef struct DeviceNode {
    uint16_t vendorID;
    uint16_t productID;
    GtkWidget *vendor_label;
    GtkWidget *product_label;
    GtkWidget *button;
    gboolean isBlocked;
    struct DeviceNode *next;
    struct DeviceNode *prev;
} DeviceNode;

static GtkWidget *progressbar;
static ssize_t device_count = 0;
static DeviceNode *table = NULL;

// Signal handler function
//static gboolean signalHandler(gpointer user_data);
void signalHandler(int signal);

// Function to handle button clicks in each row
static void button_clicked(GtkWidget *widget, gpointer data);

// Function to update table content
static void update_content(DeviceNode *table);

// Function to update the GUI based on table content
static void update_gui(DeviceNode *device);

// Function to refresh the table content
static void refresh(GtkWidget *widget, gpointer data);

DeviceNode *create_node_gui(uint16_t id_vendor,
                            uint16_t id_product,
                            gboolean is_blocked);
ExitCode delete_node_gui(DeviceNode *node);
ExitCode delete_list_gui(DeviceNode *list);
ExitCode append_gui(DeviceNode *list,
                    uint16_t id_vendor,
                    uint16_t id_product,
                    gboolean is_blocked);
ExitCode delete_gui(DeviceNode *list,
                    uint16_t id_vendor,
                    uint16_t id_product);
DeviceNode *search_gui(DeviceNode *list,
                       uint16_t id_vendor,
                       uint16_t id_product);

gint load_from_model(DeviceNode **dev_array);

int view(int argc, char *argv[]);

#endif
