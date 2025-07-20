#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include "app_data.h"

// Main window functions
void activate(GtkApplication *app, gpointer user_data);
void on_window_destroy(GtkWidget *widget, gpointer user_data);

// File chooser functions
void on_file_chooser_clicked(GtkButton *button, gpointer user_data);
void file_dialog_response(GObject *source, GAsyncResult *result, gpointer user_data);

// Sync functions
void on_sync_clicked(GtkButton *button, gpointer user_data);
void on_download_clicked(GtkButton *button, gpointer user_data);

// Configuration dialog functions
void on_config_clicked(GtkButton *button, gpointer user_data);
void show_config_dialog(AppData *app);
void on_provider_changed(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data);
void on_save_credentials_clicked(GtkButton *button, gpointer user_data);
void on_oauth_complete_clicked(GtkButton *button, gpointer user_data);
void on_complete_oauth_clicked(GtkButton *button, gpointer user_data);
void show_gdrive_instructions(GtkButton *button, gpointer user_data);
void on_url_clicked(GtkButton *button, gpointer user_data);
void cleanup_config_dialog(GtkWidget *widget, gpointer user_data);

#endif