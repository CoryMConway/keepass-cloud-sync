#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../include/cloud.h"
#include "../include/utils.h"

char* exchange_auth_code_for_token(const char *provider, const char *client_id, const char *client_secret, const char *auth_code) {
    if (strcmp(provider, "drive") == 0) {
        // For Google Drive, exchange the auth code for tokens
        char curl_command[4096];
        snprintf(curl_command, sizeof(curl_command),
            "curl -s -X POST https://oauth2.googleapis.com/token "
            "-H 'Content-Type: application/x-www-form-urlencoded' "
            "-d 'client_id=%s&client_secret=%s&code=%s&grant_type=authorization_code&redirect_uri=urn:ietf:wg:oauth:2.0:oob'",
            client_id, client_secret, auth_code);
        
        char *result = execute_command_with_output(curl_command);
        if (result && strstr(result, "access_token")) {
            return result;
        }
        if (result) g_free(result);
    }
    
    return NULL;
}

void open_oauth_url(const char *provider, const char *client_id) {
    if (strcmp(provider, "drive") == 0) {
        char oauth_url[2048];
        snprintf(oauth_url, sizeof(oauth_url), 
            "https://accounts.google.com/o/oauth2/auth"
            "?client_id=%s"
            "&redirect_uri=urn:ietf:wg:oauth:2.0:oob"
            "&scope=https://www.googleapis.com/auth/drive"
            "&response_type=code"
            "&access_type=offline",
            client_id);
        
        // Open the OAuth URL in browser
        open_url_in_browser(oauth_url);
    }
}