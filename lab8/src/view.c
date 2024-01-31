#include "../include/view.h"

pthread_t thread_model;
pthread_t thread_view;

gint load_from_model(DeviceNode **dev_array) {
    char buffer[64];
    char *ptr;

    int named_pipe;
    ssize_t buffer_len;

    named_pipe = open(pipe_name, O_RDONLY | O_NONBLOCK);
    while (device_count <= 0) {
        pthread_mutex_lock(&threadData.mutex);
        buffer_len = read(named_pipe, buffer, sizeof(buffer));
        pthread_mutex_unlock(&threadData.mutex);
        device_count = strtoul(buffer, &ptr, 10);
    }
//    pthread_mutex_lock(&threadData.mutex);
//    buffer_len = read(named_pipe, buffer, sizeof(buffer));
//    pthread_mutex_unlock(&threadData.mutex);
    device_count = strtoul(buffer, &ptr, 10);
    printf("BUFFER: %s\n", buffer);

    printf("Get this %zd\n", device_count);

    DeviceNode *table_new = NULL;
    uint16_t vendorID;
    uint16_t productID;
    gboolean isBlocked;

    for (ssize_t i = 0; i < device_count; i++) {
        pthread_mutex_lock(&threadData.mutex);
        buffer_len = read(named_pipe, buffer, sizeof(buffer));
        pthread_mutex_unlock(&threadData.mutex);
        printf("BUFFER: %s\n", buffer);
        sscanf(buffer,
               "%hu %hu %d",
               &vendorID,
               &productID,
               &isBlocked);

        printf("Get this: %hu %hu %d\n",
               vendorID,
               productID,
               isBlocked);

        if (table_new == NULL)
            table_new = create_node_gui(vendorID, productID, isBlocked);
        else
            append_gui(table_new, vendorID, productID, isBlocked);
    }

    if (table_new) {
        if (*dev_array)
            delete_list_gui(*dev_array);

        *dev_array = table_new;
    }

    close(named_pipe);

    return SUCCESS;
}

int view(int argc, char *argv[]) {

    threadData.productID = 0;
    threadData.vendorID = 0;

    thread_view = pthread_self();

    pthread_create(&thread_model, NULL, control, argv[1]);

    gtk_init(&argc, &argv);

    mkfifo(pipe_name, 0666);

    signal(SIGUSR2, signalHandler);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "USB Control");
    gtk_window_set_default_size(GTK_WINDOW(window), 320, 410);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the table and buttons
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), grid);

    // Create a progress bar
    progressbar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), progressbar, FALSE, FALSE, 0);

    // Create headers
    GtkWidget *label1 = gtk_label_new("Vendor ID\t\t");
    GtkWidget *label2 = gtk_label_new("Product ID\t\t");
    GtkWidget *label3 = gtk_label_new("Block/Unblock");

    gtk_grid_attach(GTK_GRID(grid), label1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label2, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label3, 2, 0, 2, 1);

    // Initialize table content (int, int, button in each row)
    pthread_kill(thread_model, SIGUSR1);
//    load_from_model(&table);

    int i = 0;
    for (DeviceNode *ptr = table; ptr != NULL; ptr = ptr->next) {
        ptr->button = gtk_button_new_with_label("Block");
        g_signal_connect(ptr->button,
                         "clicked",
                         G_CALLBACK(button_clicked),
                         ptr);

        // Set initial button color
        GtkCssProvider *provider = gtk_css_provider_new();
        GError *error = NULL;

        if (!gtk_css_provider_load_from_path(provider, CSS_PATH, &error)) {
            g_printerr("Error loading CSS file: %s\n", error->message);
            g_error_free(error);
            return -1;
        }
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                                  GTK_STYLE_PROVIDER (provider),
                                                  GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        GtkStyleContext
            *context = gtk_widget_get_style_context(ptr->button);
        gtk_style_context_add_class(context, "button");

        gtk_grid_attach(GTK_GRID(grid), ptr->vendor_label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), ptr->product_label, 1, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), ptr->button, 2, i + 1, 2, 1);
        i++;
    }

    // Create a refresh button
    GtkWidget *refresh_button = gtk_button_new_with_label("Refresh");
    g_signal_connect(refresh_button,
                     "clicked",
                     G_CALLBACK(refresh),
                     table);

    gtk_box_pack_start(GTK_BOX(vbox), refresh_button, FALSE, FALSE, 0);

    // Pass vbox as additional data to refresh_table function
    g_object_set_data(G_OBJECT(refresh_button), "vbox", vbox);

    gtk_widget_show_all(window);

    gtk_main();

    delete_list_gui(table);

    pthread_kill(thread_model, SIGINT);

    pthread_join(thread_model, SUCCESS);

    unlink(pipe_name);

    return 0;
}

static void button_clicked(GtkWidget *widget, gpointer data) {
    DeviceNode *row = (DeviceNode *) data;
    g_print("Button clicked (Values: %d, %d, Blocked: %s)\n",
            row->vendorID, row->productID, (row->isBlocked ? "true" : "false"));

    // Toggle the blocked state
    row->isBlocked = !row->isBlocked;

    pthread_mutex_lock(&threadData.mutex);
    threadData.vendorID = row->vendorID;
    threadData.productID = row->productID;
    pthread_mutex_unlock(&threadData.mutex);

    printf("SIGNAL FROM VIEW\n");
    pthread_kill(thread_model, SIGUSR1);

    update_gui(row);
}

static void update_content(DeviceNode *p_table) {
    for (DeviceNode *ptr = p_table; ptr != NULL; ptr = ptr->next) {
        update_gui(ptr);
//        g_usleep(100000);
        gtk_main_iteration();
    }
}

static void update_gui(DeviceNode *device) {
    gtk_label_set_text(device->vendor_label,
                       g_strdup_printf("%04x", device->vendorID));
    gtk_label_set_text(device->product_label,
                       g_strdup_printf("%04x", device->productID));

    // Update button label
    gtk_button_set_label(GTK_BUTTON(device->button),
                         device->isBlocked ? "Unblock" : "Block");

    // Clear existing classes
    GtkStyleContext *context_new = gtk_widget_get_style_context(device->button);
    gtk_style_context_remove_class(context_new, "button_blocked");
    gtk_style_context_remove_class(context_new, "button");

    // Add the appropriate class based on the blocked state
    const char *css_class = device->isBlocked ? "button_blocked" : "button";
    gtk_style_context_add_class(context_new, css_class);
}

static void refresh(GtkWidget *widget, gpointer data) {
    DeviceNode *table_new = (DeviceNode *) data;

    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressbar));
    gtk_main_iteration();

    // Your code to refresh the table goes here
    g_print("Refreshing table...\n");

    printf("SIGNAL FROM VIEW\n");
    pthread_kill(thread_model, SIGUSR1);
    update_content(table_new);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), 0);
    gtk_main_iteration();
}

// list methods
DeviceNode *create_node_gui(uint16_t id_vendor,
                            uint16_t id_product,
                            gboolean is_blocked) {
    DeviceNode *node;
    node = malloc(sizeof(DeviceNode));
    if (node == NULL)
        return NULL;

    node->prev = NULL;
    node->isBlocked = is_blocked;
    node->vendorID = id_vendor;
    node->productID = id_product;
    node->vendor_label = gtk_label_new(g_strdup_printf("%04x", node->vendorID));
    node->product_label =
        gtk_label_new(g_strdup_printf("%04x", node->productID));
    node->button = gtk_button_new_with_label("Block");
    node->next = NULL;

    return node;
}

ExitCode delete_node_gui(DeviceNode *node) {
    if (node == NULL)
        return NO_SUCH_POINTER;

    free(node);

    return SUCCESS;

}

ExitCode delete_list_gui(DeviceNode *list) {
    if (list == NULL)
        return NO_SUCH_POINTER;

    DeviceNode *current = list->next;
    while (current) {
        DeviceNode *next = current->next;
        delete_node_gui(current);
        current = next;
    }

    free(list);
    return SUCCESS;
}

ExitCode append_gui(DeviceNode *list,
                    uint16_t id_vendor,
                    uint16_t id_product,
                    gboolean is_blocked) {
    if (list == NULL)
        return NO_SUCH_POINTER;

    DeviceNode *new_node;
    new_node = create_node_gui(id_vendor, id_product, is_blocked);
    if (new_node == NULL)
        return MEMORY_ALLOCATION_FAILURE;

    DeviceNode *ptr;
    ptr = list;
    for (; ptr->next != NULL; ptr = ptr->next);

    ptr->next = new_node;
    new_node->prev = ptr;

    return SUCCESS;
}

ExitCode delete_gui(DeviceNode *list,
                    uint16_t id_vendor,
                    uint16_t id_product) {
    if (list == NULL)
        return NO_SUCH_POINTER;

    DeviceNode *ptr;
    ptr = search_gui(list, id_vendor, id_product);
    if (ptr == NULL)
        return NO_SUCH_NODE;

    if (ptr->prev != NULL)
        ptr->prev->next = ptr->next;

    if (ptr->next != NULL)
        ptr->next->prev = ptr->prev;

    delete_node_gui(ptr);

    return SUCCESS;
}

DeviceNode *search_gui(DeviceNode *list,
                       uint16_t id_vendor,
                       uint16_t id_product) {
    if (list == NULL)
        return NULL;

    DeviceNode *ptr;
    ptr = list;
    for (; ptr != NULL; ptr = ptr->next)
        if (ptr->vendorID == id_vendor && ptr->productID == id_product)
            return ptr;

    return NULL;
}

// Signal handler function
//static gboolean signalHandler(gpointer user_data) {
void signalHandler(int signal) {
    load_from_model(&table);
    update_content(table);
}