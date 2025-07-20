#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>

// File utilities
char* get_home_directory(void);
char* get_config_directory(void);
char* get_config_file_path(void);
gboolean create_directory(const char *path);
char* get_filename_from_path(const char *path);

// Process utilities
gboolean open_url_in_browser(const char *url);
char* execute_command_with_output(const char *command);

#endif