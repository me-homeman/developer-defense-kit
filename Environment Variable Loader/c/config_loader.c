#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_KEY_LENGTH 128
#define MAX_VAL_LENGTH 896

// Helper to remove leading and trailing whitespace
char* trim(char* str) {
    char* end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    *(end+1) = 0;

    return str;
}

// Function to find value in .env file
int get_env_from_file(const char * target_key, char * output_buffer) {
    FILE * file = fopen(".env", "r");
    if (file == NULL) {
        return 0; // File not found
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments
        char* trimmed_line = trim(line);
        if (trimmed_line[0] == '#' || trimmed_line[0] == '\0') continue;

        char * equals_sign = strchr(trimmed_line, '=');
        if (equals_sign) {
            *equals_sign = '\0'; // Split into key and value
            char * key = trim(trimmed_line);
            char * value = trim(equals_sign + 1);
            
            if (strcmp(key, target_key) == 0) {
                // Safe copy ensuring null termination
                strncpy(output_buffer, value, MAX_VAL_LENGTH - 1);
                output_buffer[MAX_VAL_LENGTH - 1] = '\0';
                fclose(file);
                return 1; // Found
            }
        }
    }

    fclose(file);
    return 0; // Not found
}

void get_secret(const char * key, char * output_buffer) {
    // 1. Check System Environment Variable (Priority)
    char * system_val = getenv(key);
    
    if (system_val != NULL) {
        strncpy(output_buffer, system_val, MAX_VAL_LENGTH - 1);
        output_buffer[MAX_VAL_LENGTH - 1] = '\0';
    } else {
        // 2. Check .env file
        if (!get_env_from_file(key, output_buffer)) {
            // 3. Fail Fast
            fprintf(stderr, "CRITICAL ERROR: Missing required environment variable: '%s'\n", key);
            exit(EXIT_FAILURE);
        }
    }

    // 4. Safety Check (Hardcode Prevention)
    const char * unsafe_placeholders[] = {"REPLACE_ME", "YOUR_PASSWORD_HERE", "CHANGE_THIS", NULL};
    for (int i = 0; unsafe_placeholders[i] != NULL; i++) {
        if (strcmp(output_buffer, unsafe_placeholders[i]) == 0) {
            fprintf(stderr, "SECURITY ALERT: Default placeholder detected for '%s'. Update your configuration.\n", key);
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    const char * target_key = "API_KEY";
    char secret[MAX_VAL_LENGTH];

    printf("Attempting to load %s...\n", target_key);
    get_secret(target_key, secret);

    // Mask secret for display
    char masked[MAX_VAL_LENGTH];
    int len = strlen(secret);
    if (len <= 4) {
        strcpy(masked, "****");
    } else {
        // Safe mask construction
        snprintf(masked, MAX_VAL_LENGTH, "%.2s****%.2s", secret, secret + len - 2);
    }

    printf("Success! Securely loaded %s: %s\n", target_key, masked);

    return 0;
}