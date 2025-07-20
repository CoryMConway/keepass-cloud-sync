#ifndef CLOUD_H
#define CLOUD_H

#include <gtk/gtk.h>
#include "app_data.h"

// rclone command execution
void execute_rclone_command(const char *command, GtkLabel *status_label);

// rclone configuration management
void save_config_to_rclone(const char *provider, const char *client_id, const char *client_secret, const char *token_json);

// OAuth utilities
void open_oauth_url(const char *provider, const char *client_id);
char* exchange_auth_code_for_token(const char *provider, const char *client_id, const char *client_secret, const char *auth_code);

#endif