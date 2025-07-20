#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "../include/utils.h"

char* get_home_directory(void) {
    return getenv("HOME");
}

char* get_config_directory(void) {
    char *home = get_home_directory();
    if (!home) return NULL;
    
    char *config_dir = g_malloc(strlen(home) + 20);
    snprintf(config_dir, strlen(home) + 20, "%s/.config/rclone", home);
    return config_dir;
}

char* get_config_file_path(void) {
    char *config_dir = get_config_directory();
    if (!config_dir) return NULL;
    
    char *config_file = g_malloc(strlen(config_dir) + 15);
    snprintf(config_file, strlen(config_dir) + 15, "%s/rclone.conf", config_dir);
    g_free(config_dir);
    return config_file;
}

gboolean create_directory(const char *path) {
    char *mkdir_argv[4];
    mkdir_argv[0] = "mkdir";
    mkdir_argv[1] = "-p";
    mkdir_argv[2] = (char*)path;
    mkdir_argv[3] = NULL;
    
    GError *error = NULL;
    gboolean success = g_spawn_sync(NULL, mkdir_argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, NULL, &error);
    if (error) {
        g_warning("Failed to create directory: %s", error->message);
        g_error_free(error);
        return FALSE;
    }
    return success;
}

char* get_filename_from_path(const char *path) {
    return g_path_get_basename(path);
}