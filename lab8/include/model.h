#ifndef MODEL_H
#define MODEL_H

#include <glib.h>

typedef struct _ChecksumManager ChecksumManager;

ChecksumManager *checksum_manager_new();
void checksum_manager_destroy(ChecksumManager *manager);
void checksum_manager_set_directory(ChecksumManager *manager, const char *directory);
void checksum_manager_set_update_interval(ChecksumManager *manager, guint seconds);
void checksum_manager_start(ChecksumManager *manager);
void checksum_manager_stop(ChecksumManager *manager);

#define CHECKSUM_UPDATED "checksum-updated"
#endif  // MODEL_H
