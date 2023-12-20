#include "../include/model.h"

typedef struct _ChecksumManager {
    GObject parent;
    gchar *directory;
    guint update_interval;
    GHashTable *checksums;
    GTimer *timer;
} ChecksumManager;

G_DEFINE_TYPE(ChecksumManager, CHECKSUM_MANAGER, G_TYPE_OBJECT)

static void checksum_manager_init(ChecksumManager *self) {
    self->directory = NULL;
    self->update_interval = 0;
    self->checksums = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    self->timer = g_timer_new();
}

static void checksum_manager_finalize(GObject *object) {
    ChecksumManager *self = CHECKSUM_MANAGER(object);

    g_free(self->directory);
    g_hash_table_destroy(self->checksums);
    g_timer_destroy(self->timer);

    G_OBJECT_CLASS(checksum_manager_parent_class)->finalize(object);
}

static void checksum_manager_update_all(ChecksumManager *self) {
    g_hash_table_remove_all(self->checksums);

    // Update checksums here
    // ...

    g_signal_emit(self, CHECKSUM_UPDATED, 0);
}

static void checksum_manager_timer_callback(gpointer data) {
    ChecksumManager *self = CHECKSUM_MANAGER(data);

    checksum_manager_update_all(self);
}

ChecksumManager *checksum_manager_new() {
    return g_object_new(CHECKSUM_MANAGER, NULL);
}

void checksum_manager_destroy(ChecksumManager *manager) {
    g_tree_unref(manager);
}

void checksum_manager_set_directory(ChecksumManager *manager, const char *directory) {
    g_free(manager->directory);
    manager->directory = g_strdup(directory);
}

void checksum_manager_set_update_interval(ChecksumManager *manager, guint seconds) {
    manager->update_interval = seconds;
    if (manager->timer) {
        g_timer_stop(manager->timer);
    }
    if (manager->update_interval > 0) {
        g_timer_start(manager->timer);
        g_timer_set_interval(manager->timer, manager->update_interval * 1000, checksum_manager_timer_callback, manager);
    }
}

void checksum_manager_start(ChecksumManager *manager) {
    if (!manager->timer || !manager->update_interval) {
        checksum_manager_set_update_interval(manager, 30 * 60);  // Set default interval
    }
}

void checksum_manager_stop(ChecksumManager *manager) {
    if (manager->timer) {
        g_timer_stop(manager->timer);
    }
}