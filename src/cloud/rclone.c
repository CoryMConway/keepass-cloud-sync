#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../include/cloud.h"

void execute_rclone_command(const char *command, GtkLabel *status_label) {
    gtk_label_set_text(status_label, "Executing rclone command...");
    
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        gtk_label_set_text(status_label, "Error: Failed to execute rclone");
        return;
    }
    
    char buffer[1024];
    GString *output = g_string_new("");
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        g_string_append(output, buffer);
    }
    
    int status = pclose(fp);
    
    if (status == 0) {
        gtk_label_set_text(status_label, "Sync completed successfully");
    } else {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Error: rclone failed with exit code %d", status);
        gtk_label_set_text(status_label, error_msg);
    }
    
    g_string_free(output, TRUE);
}