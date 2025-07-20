#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/app_data.h"
#include "../include/ui.h"
#include "../include/cloud.h"
#include "../include/utils.h"

void on_oauth_complete_clicked(GtkButton *button, gpointer user_data) {
    ConfigDialog *config = (ConfigDialog *)user_data;
    const char *client_id = gtk_editable_get_text(GTK_EDITABLE(config->client_id_entry));
    const char *client_secret = gtk_editable_get_text(GTK_EDITABLE(config->client_secret_entry));
    
    if (strlen(client_id) == 0 || strlen(client_secret) == 0) {
        gtk_label_set_text(GTK_LABEL(config->status_label), "Please save credentials first!");
        return;
    }
    
    gtk_label_set_text(GTK_LABEL(config->status_label), "Opening OAuth URL in browser...");
    
    // Create the OAuth URL manually for Google Drive
    if (strcmp(config->current_provider, "drive") == 0) {
        open_oauth_url(config->current_provider, client_id);
        
        gtk_label_set_text(GTK_LABEL(config->status_label), 
            "OAuth URL opened in browser. Copy the authorization code and enter it above, then click 'Complete Setup'.");
        
        // Hide OAuth button, show complete button and auth code entry
        gtk_widget_set_visible(config->oauth_button, FALSE);
        gtk_widget_set_visible(config->auth_code_label, TRUE);
        gtk_widget_set_visible(config->auth_code_entry, TRUE);
        gtk_widget_set_visible(config->complete_button, TRUE);
    } else {
        gtk_label_set_text(GTK_LABEL(config->status_label), 
            "OAuth setup for this provider not yet implemented. Please use 'rclone config' manually.");
    }
}

void on_complete_oauth_clicked(GtkButton *button, gpointer user_data) {
    ConfigDialog *config = (ConfigDialog *)user_data;
    const char *auth_code = gtk_editable_get_text(GTK_EDITABLE(config->auth_code_entry));
    const char *client_id = gtk_editable_get_text(GTK_EDITABLE(config->client_id_entry));
    const char *client_secret = gtk_editable_get_text(GTK_EDITABLE(config->client_secret_entry));
    
    if (strlen(auth_code) == 0) {
        gtk_label_set_text(GTK_LABEL(config->status_label), "Please enter the authorization code");
        return;
    }
    
    gtk_label_set_text(GTK_LABEL(config->status_label), "Completing OAuth setup...");
    
    // Exchange authorization code for access token
    if (strcmp(config->current_provider, "drive") == 0) {
        char *token_json = exchange_auth_code_for_token(config->current_provider, client_id, client_secret, auth_code);
        
        if (token_json && strstr(token_json, "access_token")) {
            // Token exchange successful - save configuration with token
            save_config_to_rclone(config->current_provider, client_id, client_secret, token_json);
            
            gtk_label_set_text(GTK_LABEL(config->status_label), 
                "OAuth setup completed successfully! You can now use this provider for syncing.");
            
            // Store success in app data
            if (config->app->drive_token) g_free(config->app->drive_token);
            config->app->drive_token = g_strdup("configured");
            
            // Hide OAuth UI elements since setup is complete
            gtk_widget_set_visible(config->oauth_button, FALSE);
            gtk_widget_set_visible(config->auth_code_label, FALSE);
            gtk_widget_set_visible(config->auth_code_entry, FALSE);
            gtk_widget_set_visible(config->complete_button, FALSE);
            
            g_free(token_json);
        } else {
            // Token exchange failed - provide helpful error message
            gtk_label_set_text(GTK_LABEL(config->status_label), 
                "OAuth failed. Please check your authorization code and try again.");
            if (token_json) g_free(token_json);
        }
    } else {
        // For other providers, save basic configuration for now
        save_config_to_rclone(config->current_provider, client_id, client_secret, "");
        gtk_label_set_text(GTK_LABEL(config->status_label), 
            "Configuration saved! OAuth for this provider will be implemented soon.");
    }
}

void on_save_credentials_clicked(GtkButton *button, gpointer user_data) {
    ConfigDialog *config = (ConfigDialog *)user_data;
    const char *client_id = gtk_editable_get_text(GTK_EDITABLE(config->client_id_entry));
    const char *client_secret = gtk_editable_get_text(GTK_EDITABLE(config->client_secret_entry));
    
    if (strlen(client_id) == 0 || strlen(client_secret) == 0) {
        gtk_label_set_text(GTK_LABEL(config->status_label), "Please enter Client ID and Client Secret");
        return;
    }
    
    gtk_label_set_text(GTK_LABEL(config->status_label), "Saving credentials...");
    
    // Save the configuration with credentials
    save_config_to_rclone(config->current_provider, client_id, client_secret, "");
    
    gtk_label_set_text(GTK_LABEL(config->status_label), 
        "Credentials saved! Now click 'Complete OAuth Setup' to finish.");
    
    // Show the OAuth button
    gtk_widget_set_visible(config->oauth_button, TRUE);
}

void show_gdrive_instructions(GtkButton *button, gpointer user_data) {
    GtkWidget *parent_window = (GtkWidget *)user_data;
    
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Google Drive Setup Instructions");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 500);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_widget_set_margin_end(vbox, 20);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    
    // Instructions
    const char *instructions[] = {
        "📋 Google Drive API Setup Steps:",
        "",
        "1️⃣ CREATE PROJECT:",
        "   • Go to Google Cloud Console",
        "   • Click 'Select a project' → 'New Project'",
        "   • Name it 'KeePass Sync' → Create",
        "",
        "2️⃣ ENABLE DRIVE API:",
        "   • Go to 'APIs & Services' → 'Library'",
        "   • Search for 'Google Drive API'",
        "   • Click on it → Click 'Enable'",
        "",
        "3️⃣ CREATE CREDENTIALS:",
        "   • Go to 'APIs & Services' → 'Credentials'",
        "   • Click '+ Create Credentials' → 'OAuth client ID'",
        "   • Choose 'Desktop application'",
        "   • Name it 'KeePass Sync'",
        "",
        "4️⃣ CONFIGURE OAUTH:",
        "   • If prompted about OAuth consent screen:",
        "     - Click 'Configure consent screen'",
        "     - Choose 'External' → Create",
        "     - Fill App name: 'KeePass Sync'",
        "     - Add your email as developer contact",
        "     - Save and continue through all steps",
        "",
        "5️⃣ GET CREDENTIALS:",
        "   • After creating OAuth client:",
        "   • Copy the 'Client ID' and 'Client secret'",
        "   • Paste them in the app fields",
        "",
        "6️⃣ ADD TEST USER (IMPORTANT):",
        "   • Go to 'APIs & Services' → 'OAuth consent screen'",
        "   • Scroll to 'Test users' section",
        "   • Click '+ ADD USERS'",
        "   • Enter your Gmail address → Save",
        "",
        "7️⃣ GET AUTHORIZATION CODE:",
        "   • After entering Client ID/Secret, click 'Get Auth Code'",
        "   • Browser opens → Sign in to Google",
        "   • Grant permission to access Drive",
        "   • Copy the authorization code shown",
        "   • Paste it back in the app",
        "",
        "⚠️  If you get 'access blocked' error, make sure you",
        "    added your email as a test user in step 6!",
        "",
        "✅ You're done! The app will handle the rest."
    };
    
    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
        GtkWidget *label = gtk_label_new(instructions[i]);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_label_set_selectable(GTK_LABEL(label), TRUE);
        gtk_box_append(GTK_BOX(vbox), label);
    }
    
    GtkWidget *close_button = gtk_button_new_with_label("Got it!");
    g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_box_append(GTK_BOX(vbox), close_button);
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), vbox);
    gtk_window_set_child(GTK_WINDOW(dialog), scrolled);
    gtk_window_present(GTK_WINDOW(dialog));
}

void on_url_clicked(GtkButton *button, gpointer user_data) {
    const char *url = (const char *)user_data;
    open_url_in_browser(url);
}

void on_provider_changed(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data) {
    ConfigDialog *config = (ConfigDialog *)user_data;
    guint selected = gtk_drop_down_get_selected(dropdown);
    
    const char *providers[] = {"drive", "dropbox", "onedrive", "box", "pcloud"};
    const char *auth_urls[] = {
        "https://console.developers.google.com/",
        "https://www.dropbox.com/developers/apps",
        "https://developer.microsoft.com/en-us/microsoft-365/dev-program",
        "https://app.box.com/developers/console",
        "https://docs.pcloud.com/methods/intro/authentication.html"
    };
    
    if (selected < 5) {
        // Free previous provider string if it exists
        if (config->current_provider) {
            g_free(config->current_provider);
        }
        config->current_provider = g_strdup(providers[selected]);
        
        // Update the button label and callback
        char button_text[512];
        if (selected == 0) { // Google Drive
            snprintf(button_text, sizeof(button_text), "Open Google Cloud Console");
            gtk_label_set_text(GTK_LABEL(config->status_label), 
                "Click 'Show Detailed Instructions' for step-by-step Google Drive setup guide");
            if (config->help_button) {
                gtk_widget_set_visible(config->help_button, TRUE);
                gtk_button_set_label(GTK_BUTTON(config->help_button), "Show Detailed Instructions");
            }
        } else if (selected == 2) { // OneDrive
            snprintf(button_text, sizeof(button_text), "Join M365 Developer Program (Required)");
            gtk_label_set_text(GTK_LABEL(config->status_label), 
                "OneDrive requires M365 Developer Program (free). After joining, go to portal.azure.com to register your app.");
            if (config->help_button) {
                gtk_widget_set_visible(config->help_button, FALSE);
            }
        } else {
            snprintf(button_text, sizeof(button_text), "Open %s Developer Console", providers[selected]);
            gtk_label_set_text(GTK_LABEL(config->status_label), "Ready to configure");
            if (config->help_button) {
                gtk_widget_set_visible(config->help_button, FALSE);
            }
        }
        
        gtk_button_set_label(GTK_BUTTON(config->auth_url_button), button_text);
        
        // Update the button's click handler with the new URL
        g_signal_handlers_disconnect_by_func(config->auth_url_button, G_CALLBACK(on_url_clicked), NULL);
        g_signal_connect(config->auth_url_button, "clicked", G_CALLBACK(on_url_clicked), (gpointer)auth_urls[selected]);
        
        // Clear previous entries
        gtk_editable_set_text(GTK_EDITABLE(config->client_id_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(config->client_secret_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(config->auth_code_entry), "");
    }
}

void cleanup_config_dialog(GtkWidget *widget, gpointer user_data) {
    ConfigDialog *config = (ConfigDialog *)user_data;
    if (config->current_provider) {
        g_free(config->current_provider);
    }
    g_free(config);
}

void show_config_dialog(AppData *app) {
    ConfigDialog *config = g_malloc0(sizeof(ConfigDialog)); // Use g_malloc0 to zero-initialize
    config->app = app;
    config->current_provider = NULL;
    
    config->dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(config->dialog), "Cloud Provider Configuration");
    gtk_window_set_default_size(GTK_WINDOW(config->dialog), 500, 400);
    gtk_window_set_transient_for(GTK_WINDOW(config->dialog), GTK_WINDOW(app->window));
    gtk_window_set_modal(GTK_WINDOW(config->dialog), TRUE);
    
    // Connect cleanup function
    g_signal_connect(config->dialog, "destroy", G_CALLBACK(cleanup_config_dialog), config);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_widget_set_margin_end(vbox, 20);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    
    // Provider selection
    GtkWidget *provider_label = gtk_label_new("Select Cloud Provider:");
    gtk_widget_set_halign(provider_label, GTK_ALIGN_START);
    
    GtkStringList *provider_list = gtk_string_list_new(NULL);
    gtk_string_list_append(provider_list, "Google Drive");
    gtk_string_list_append(provider_list, "Dropbox");
    gtk_string_list_append(provider_list, "OneDrive");
    gtk_string_list_append(provider_list, "Box");
    gtk_string_list_append(provider_list, "pCloud");
    
    config->provider_combo = gtk_drop_down_new(G_LIST_MODEL(provider_list), NULL);
    g_signal_connect(config->provider_combo, "notify::selected", G_CALLBACK(on_provider_changed), config);
    
    // Instructions as a clickable button
    GtkWidget *url_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    config->auth_url_button = gtk_button_new_with_label("Select a provider to see instructions");
    gtk_widget_set_halign(config->auth_url_button, GTK_ALIGN_START);
    
    config->help_button = gtk_button_new_with_label("Show Instructions");
    gtk_widget_set_visible(config->help_button, FALSE); // Hidden initially
    g_signal_connect(config->help_button, "clicked", G_CALLBACK(show_gdrive_instructions), config->dialog);
    
    gtk_box_append(GTK_BOX(url_button_box), config->auth_url_button);
    gtk_box_append(GTK_BOX(url_button_box), config->help_button);
    
    // Client ID
    GtkWidget *client_id_label = gtk_label_new("Client ID:");
    gtk_widget_set_halign(client_id_label, GTK_ALIGN_START);
    config->client_id_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(config->client_id_entry), "Enter your app's Client ID");
    
    // Client Secret
    GtkWidget *client_secret_label = gtk_label_new("Client Secret:");
    gtk_widget_set_halign(client_secret_label, GTK_ALIGN_START);
    config->client_secret_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(config->client_secret_entry), "Enter your app's Client Secret");
    gtk_entry_set_visibility(GTK_ENTRY(config->client_secret_entry), FALSE);
    
    // Authorization code entry (hidden initially)
    config->auth_code_label = gtk_label_new("Authorization Code:");
    gtk_widget_set_halign(config->auth_code_label, GTK_ALIGN_START);
    gtk_widget_set_visible(config->auth_code_label, FALSE);
    config->auth_code_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(config->auth_code_entry), "Paste authorization code here");
    gtk_widget_set_visible(config->auth_code_entry, FALSE);
    
    // OAuth completion button
    config->oauth_button = gtk_button_new_with_label("Get Authorization Code");
    gtk_widget_set_visible(config->oauth_button, FALSE); // Hidden initially
    g_signal_connect(config->oauth_button, "clicked", G_CALLBACK(on_oauth_complete_clicked), config);
    
    // Complete setup button (hidden initially)
    config->complete_button = gtk_button_new_with_label("Complete Setup");
    gtk_widget_set_visible(config->complete_button, FALSE);
    g_signal_connect(config->complete_button, "clicked", G_CALLBACK(on_complete_oauth_clicked), config);
    
    // Status
    config->status_label = gtk_label_new("Select a provider to begin configuration");
    gtk_widget_set_halign(config->status_label, GTK_ALIGN_START);
    
    // Buttons
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *save_button = gtk_button_new_with_label("Save Credentials");
    GtkWidget *close_button = gtk_button_new_with_label("Close");
    
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_credentials_clicked), config);
    g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), config->dialog);
    
    gtk_box_append(GTK_BOX(vbox), provider_label);
    gtk_box_append(GTK_BOX(vbox), config->provider_combo);
    gtk_box_append(GTK_BOX(vbox), url_button_box);
    gtk_box_append(GTK_BOX(vbox), client_id_label);
    gtk_box_append(GTK_BOX(vbox), config->client_id_entry);
    gtk_box_append(GTK_BOX(vbox), client_secret_label);
    gtk_box_append(GTK_BOX(vbox), config->client_secret_entry);
    gtk_box_append(GTK_BOX(vbox), config->oauth_button);
    gtk_box_append(GTK_BOX(vbox), config->auth_code_label);
    gtk_box_append(GTK_BOX(vbox), config->auth_code_entry);
    gtk_box_append(GTK_BOX(vbox), config->complete_button);
    gtk_box_append(GTK_BOX(vbox), config->status_label);
    
    gtk_box_append(GTK_BOX(button_box), save_button);
    gtk_box_append(GTK_BOX(button_box), close_button);
    gtk_box_append(GTK_BOX(vbox), button_box);
    
    gtk_window_set_child(GTK_WINDOW(config->dialog), vbox);
    gtk_window_present(GTK_WINDOW(config->dialog));
    
    // Trigger initial setup for Google Drive (default selection)
    on_provider_changed(GTK_DROP_DOWN(config->provider_combo), NULL, config);
}

void on_config_clicked(GtkButton *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    show_config_dialog(app);
}