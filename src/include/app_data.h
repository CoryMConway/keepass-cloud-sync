#ifndef APP_DATA_H
#define APP_DATA_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *file_chooser_button;
    GtkWidget *provider_dropdown;
    GtkWidget *remote_path_entry;
    GtkWidget *sync_button;
    GtkWidget *config_button;
    GtkWidget *status_label;
    char *selected_file;
    GtkStringList *provider_list;
    
    // Configuration storage
    char *drive_token;
    char *dropbox_token;
    char *onedrive_token;
    char *box_token;
    char *pcloud_token;
} AppData;

typedef struct {
    AppData *app;
    GtkWidget *dialog;
    GtkWidget *provider_combo;
    GtkWidget *client_id_entry;
    GtkWidget *client_secret_entry;
    GtkWidget *auth_url_button;
    GtkWidget *help_button;
    GtkWidget *auth_code_label;
    GtkWidget *auth_code_entry;
    GtkWidget *oauth_button;
    GtkWidget *complete_button;
    GtkWidget *status_label;
    char *current_provider;
} ConfigDialog;

void cleanup_app_data(AppData *app_data);

#endif