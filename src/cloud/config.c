#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include "../include/cloud.h"
#include "../include/utils.h"

void save_config_to_rclone(const char *provider, const char *client_id, const char *client_secret, const char *token_json) {
    char *config_dir = get_config_directory();
    char *config_file = get_config_file_path();
    
    if (!config_dir || !config_file) {
        g_warning("Failed to get config paths");
        if (config_dir) g_free(config_dir);
        if (config_file) g_free(config_file);
        return;
    }
    
    // Create config directory if it doesn't exist
    create_directory(config_dir);
    
    // Read existing config
    FILE *file = fopen(config_file, "r");
    GString *config_content = g_string_new("");
    
    if (file) {
        char line[1024];
        gboolean in_provider_section = FALSE;
        char current_section[256] = "";
        
        while (fgets(line, sizeof(line), file)) {
            // Check if we're entering a section
            if (line[0] == '[') {
                sscanf(line, "[%255[^]]", current_section);
                in_provider_section = (strcmp(current_section, provider) == 0);
            }
            
            // Skip lines in the provider section we're updating
            if (!in_provider_section) {
                g_string_append(config_content, line);
            }
        }
        fclose(file);
    }
    
    // Add our provider configuration
    g_string_append_printf(config_content, "\n[%s]\n", provider);
    g_string_append_printf(config_content, "type = %s\n", provider);
    g_string_append_printf(config_content, "client_id = %s\n", client_id);
    g_string_append_printf(config_content, "client_secret = %s\n", client_secret);
    
    // If we have token JSON, format it properly for rclone (single line, no spaces)
    if (token_json && strlen(token_json) > 0) {
        // Remove all whitespace from JSON token for rclone compatibility
        GString *clean_token = g_string_new("");
        const char *p = token_json;
        while (*p) {
            if (*p != ' ' && *p != '\n' && *p != '\r' && *p != '\t') {
                g_string_append_c(clean_token, *p);
            }
            p++;
        }
        g_string_append_printf(config_content, "token = %s\n", clean_token->str);
        g_string_free(clean_token, TRUE);
    }
    g_string_append(config_content, "\n");
    
    // Write back to file
    file = fopen(config_file, "w");
    if (file) {
        fputs(config_content->str, file);
        fclose(file);
    }
    
    g_string_free(config_content, TRUE);
    g_free(config_dir);
    g_free(config_file);
}