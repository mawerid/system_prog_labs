#ifndef VIEW_H
#define VIEW_H

#include <gtk/gtk.h>

#include "model.h"

typedef struct _ChecksumGUI ChecksumGUI;

ChecksumGUI *checksum_gui_new(ChecksumManager *manager);
void checksum_gui_destroy(ChecksumGUI *gui);
GtkWidget *checksum_gui_get_widget(ChecksumGUI *gui);

void checksum_gui_update_all(ChecksumGUI *gui, gpointer data);

#endif  // VIEW_H
