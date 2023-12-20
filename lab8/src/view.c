#include "../include/view.h"

#include <gtk/gtk.h>

typedef struct _ChecksumGUI {
    GtkWidget *widget;
    GtkWidget *list;
    ChecksumManager *manager;
} ChecksumGUI;

static GtkWidget *create_list(void) {
    GtkWidget *list = gtk_tree_view_new();

    // Create columns for file name, checksum, and last modified time
    GtkTreeModel *model = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkCellRenderer *renderer;

    // File name
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list), 0, "File Name", renderer, "text", 0, NULL);

    // Checksum
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list), 1, "Checksum", renderer, "text", 1, NULL);

    // Last modified time
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list), 2, "Last Modified", renderer, "text", 2, NULL);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list), model);
    g_object_unref(model);

    return list;
}

ChecksumGUI *checksum_gui_new(ChecksumManager *manager) {
    ChecksumGUI *gui = g_malloc(sizeof(ChecksumGUI));

    gui->manager = manager;
    gui->widget = gtk_hbox_new(GTK_ORIENTATION_VERTICAL, 0);

    gui->list = create_list();
    gtk_box_pack_start(GTK_BOX(gui->widget), gui->list, TRUE, TRUE, 0);

    g_signal_connect(manager, CHECKSUM_UPDATED, G_CALLBACK(checksum_gui_update_all), gui);

    return gui;
}

void checksum_gui_destroy(ChecksumGUI *gui) {
    g_free(gui);
}

GtkWidget *checksum_gui_get_widget(ChecksumGUI *gui) {
    return gui->widget;
}

void checksum_gui_update_all(ChecksumGUI *gui, gpointer data) {
    // Update GUI elements based on updated checksums
    // ...
}
