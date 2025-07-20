#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/app_data.h"
#include "../include/ui.h"
#include "../include/cloud.h"
#include "../include/utils.h"

void file_dialog_response(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    AppData *app = (AppData *)user_data;
    GError *error = NULL;
    
    GFile *file = gtk_file_dialog_open_finish(dialog, result, &error);
    if (file) {
        if (app->selected_file) {
            g_free(app->selected_file);
        }
        app->selected_file = g_file_get_path(file);
        g_object_unref(file);
        
        char *filename = get_filename_from_path(app->selected_file);
        char button_text[256];
        snprintf(button_text, sizeof(button_text), "Selected: %s", filename);
        gtk_button_set_label(GTK_BUTTON(app->file_chooser_button), button_text);
        g_free(filename);
        
        gtk_label_set_text(GTK_LABEL(app->status_label), "File selected");
    }
    if (error) {
        g_error_free(error);
    }
    
    g_object_unref(dialog);
}

void on_file_chooser_clicked(GtkButton *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Select KeePass Database");
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "KeePass Database Files");
    gtk_file_filter_add_pattern(filter, "*.kdbx");
    
    GListStore *filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, filter);
    gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filters));
    
    gtk_file_dialog_open(dialog, GTK_WINDOW(app->window), NULL, file_dialog_response, app);
    
    g_object_unref(filter);
    g_object_unref(filters);
}

void on_sync_clicked(GtkButton *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    
    if (!app->selected_file) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Error: No file selected");
        return;
    }
    
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(app->provider_dropdown));
    const char *provider = gtk_string_list_get_string(app->provider_list, selected);
    const char *remote_path = gtk_editable_get_text(GTK_EDITABLE(app->remote_path_entry));
    
    if (!provider || !remote_path || strlen(remote_path) == 0) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Error: Please select provider and enter remote path");
        return;
    }
    
    char command[1024];
    snprintf(command, sizeof(command), "rclone copy \"%s\" \"%s:%s\"", 
             app->selected_file, provider, remote_path);
    
    execute_rclone_command(command, GTK_LABEL(app->status_label));
}

void on_download_clicked(GtkButton *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(app->provider_dropdown));
    const char *provider = gtk_string_list_get_string(app->provider_list, selected);
    const char *remote_path = gtk_editable_get_text(GTK_EDITABLE(app->remote_path_entry));
    
    if (!provider || !remote_path || strlen(remote_path) == 0) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Error: Please select provider and enter remote path");
        return;
    }
    
    char command[1024];
    char *home_dir = get_home_directory();
    snprintf(command, sizeof(command), "rclone copy \"%s:%s\" \"%s/\"", 
             provider, remote_path, home_dir);
    
    execute_rclone_command(command, GTK_LABEL(app->status_label));
}

void cleanup_app_data(AppData *app_data) {
    if (app_data->selected_file) {
        g_free(app_data->selected_file);
    }
    if (app_data->provider_list) {
        g_object_unref(app_data->provider_list);
    }
    if (app_data->drive_token) g_free(app_data->drive_token);
    if (app_data->dropbox_token) g_free(app_data->dropbox_token);
    if (app_data->onedrive_token) g_free(app_data->onedrive_token);
    if (app_data->box_token) g_free(app_data->box_token);
    if (app_data->pcloud_token) g_free(app_data->pcloud_token);
    g_free(app_data);
}

void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    cleanup_app_data(app_data);
}

void activate(GtkApplication *app, gpointer user_data) {
    AppData *app_data = g_malloc(sizeof(AppData));
    app_data->selected_file = NULL;
    app_data->drive_token = NULL;
    app_data->dropbox_token = NULL;
    app_data->onedrive_token = NULL;
    app_data->box_token = NULL;
    app_data->pcloud_token = NULL;
    
    app_data->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(app_data->window), "KeePass Cloud Sync");
    gtk_window_set_default_size(GTK_WINDOW(app_data->window), 500, 300);
    
    g_signal_connect(app_data->window, "destroy", G_CALLBACK(on_window_destroy), app_data);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_widget_set_margin_end(vbox, 20);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    
    GtkWidget *file_label = gtk_label_new("Select KeePass Database (.kdbx):");
    gtk_widget_set_halign(file_label, GTK_ALIGN_START);
    
    app_data->file_chooser_button = gtk_button_new_with_label("Select File");
    g_signal_connect(app_data->file_chooser_button, "clicked", G_CALLBACK(on_file_chooser_clicked), app_data);
    
    // Provider section with config button
    GtkWidget *provider_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *provider_label = gtk_label_new("Cloud Provider:");
    gtk_widget_set_halign(provider_label, GTK_ALIGN_START);
    
    app_data->provider_list = gtk_string_list_new(NULL);
    gtk_string_list_append(app_data->provider_list, "drive");
    gtk_string_list_append(app_data->provider_list, "dropbox");
    gtk_string_list_append(app_data->provider_list, "onedrive");
    gtk_string_list_append(app_data->provider_list, "box");
    gtk_string_list_append(app_data->provider_list, "pcloud");
    
    app_data->provider_dropdown = gtk_drop_down_new(G_LIST_MODEL(app_data->provider_list), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(app_data->provider_dropdown), 0);
    
    app_data->config_button = gtk_button_new_with_label("Configure Cloud Providers");
    g_signal_connect(app_data->config_button, "clicked", G_CALLBACK(on_config_clicked), app_data);
    
    gtk_box_append(GTK_BOX(provider_box), app_data->provider_dropdown);
    gtk_box_append(GTK_BOX(provider_box), app_data->config_button);
    
    GtkWidget *path_label = gtk_label_new("Remote Path (e.g., /KeePass/):");
    gtk_widget_set_halign(path_label, GTK_ALIGN_START);
    
    app_data->remote_path_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(app_data->remote_path_entry), "/KeePass/");
    
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    app_data->sync_button = gtk_button_new_with_label("Upload to Cloud");
    g_signal_connect(app_data->sync_button, "clicked", G_CALLBACK(on_sync_clicked), app_data);
    
    GtkWidget *download_button = gtk_button_new_with_label("Download from Cloud");
    g_signal_connect(download_button, "clicked", G_CALLBACK(on_download_clicked), app_data);
    
    app_data->status_label = gtk_label_new("Ready");
    gtk_widget_set_halign(app_data->status_label, GTK_ALIGN_START);
    
    gtk_box_append(GTK_BOX(vbox), file_label);
    gtk_box_append(GTK_BOX(vbox), app_data->file_chooser_button);
    gtk_box_append(GTK_BOX(vbox), provider_label);
    gtk_box_append(GTK_BOX(vbox), provider_box);
    gtk_box_append(GTK_BOX(vbox), path_label);
    gtk_box_append(GTK_BOX(vbox), app_data->remote_path_entry);
    
    gtk_box_append(GTK_BOX(button_box), app_data->sync_button);
    gtk_box_append(GTK_BOX(button_box), download_button);
    gtk_box_append(GTK_BOX(vbox), button_box);
    
    gtk_box_append(GTK_BOX(vbox), app_data->status_label);
    
    gtk_window_set_child(GTK_WINDOW(app_data->window), vbox);
    gtk_window_present(GTK_WINDOW(app_data->window));
}