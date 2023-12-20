// #include "../include/viewmodel.h"

// #include <gtk/gtk.h>

// #include "../include/model.h"

// struct _AppViewModel {
//     AppModel* model;
//     GtkWidget* view;
// };

// AppViewModel* app_view_model_new(AppModel* model) {
//     AppViewModel* view_model = g_new0(AppViewModel, 1);
//     view_model->model = g_object_ref(model);
//     view_model->view = app_view_new(view_model);
//     return view_model;
// }

// gboolean app_view_model_update_checksums(AppViewModel* view_model) {
//     app_model_update_checksums(view_model->model);
//     app_view_update(view_model->view);
//     return TRUE;
// }

// void app_view_model_on_modify_event(GtkWidget* widget, gpointer data) {
//     AppViewModel* view_model = (AppViewModel*)data;
//     guint selected_row = gtk_tree_selection_get_selected_row_number(gtk_tree_view_get_selection(GTK_TREE_VIEW(view_model->view)));

//     if (selected_row < view_model->model->file_infos->len) {
//         FileInfo* file_info = &g_array_index(view_model->model->file_infos, FileInfo, selected_row);

//         if (file_info->error == FALSE) {
//             file_info->error = TRUE;
//             app_model_calculate_checksum(file_info->file_path, file_info->checksum);
//         }
//     }

//     app_view_update(view_model->view);
// }

// void app_view_model_free(AppViewModel* view_model) {
//     g_object_unref(view_model->model);
//     g_free(view_model);
// }