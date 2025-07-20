#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../include/utils.h"

gboolean open_url_in_browser(const char *url) {
    char *argv[3];
    argv[0] = "xdg-open";
    argv[1] = (char*)url;
    argv[2] = NULL;
    
    GError *error = NULL;
    gboolean success = g_spawn_async(
        NULL,           // working_directory
        argv,           // argv
        NULL,           // envp
        G_SPAWN_SEARCH_PATH, // flags
        NULL,           // child_setup
        NULL,           // user_data
        NULL,           // child_pid
        &error          // error
    );
    
    if (!success) {
        g_warning("Failed to open URL: %s", error ? error->message : "Unknown error");
        if (error) g_error_free(error);
        return FALSE;
    }
    return TRUE;
}

char* execute_command_with_output(const char *command) {
    char *curl_argv[4];
    curl_argv[0] = "sh";
    curl_argv[1] = "-c";
    curl_argv[2] = (char*)command;
    curl_argv[3] = NULL;
    
    GError *error = NULL;
    gchar *standard_output = NULL;
    gchar *standard_error = NULL;
    gint exit_status;
    
    gboolean success = g_spawn_sync(
        NULL,                    // working_directory
        curl_argv,               // argv
        NULL,                   // envp
        G_SPAWN_SEARCH_PATH,    // flags
        NULL,                   // child_setup
        NULL,                   // user_data
        &standard_output,       // standard_output
        &standard_error,        // standard_error
        &exit_status,           // exit_status
        &error                  // error
    );
    
    if (success && exit_status == 0 && standard_output) {
        char *result = g_strdup(standard_output);
        if (standard_error) g_free(standard_error);
        if (error) g_error_free(error);
        g_free(standard_output);
        return result;
    }
    
    // Clean up on failure
    if (standard_output) g_free(standard_output);
    if (standard_error) g_free(standard_error);
    if (error) g_error_free(error);
    return NULL;
}