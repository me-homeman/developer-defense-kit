#include <stdio.h>
#include <time.h>
#include <string.h>

/**
 * Writes a sanitized string to the file.
 * Replaces newlines and control characters to prevent log injection.
 */
void write_sanitized(FILE* file, const char* input) {
    if (input == NULL) return;
    while (*input) {
        if (*input == '\n') {
            fputs("\\n", file);
        } else if (*input == '\r') {
            fputs("\\r", file);
        } else if (*input == '\t') {
            fputs("\\t", file);
        } else if (*input >= 0 && *input < 32) {
            fputc(' ', file);
        } else {
            fputc(*input, file);
        }
        input++;
    }
}

/**
 * Logs suspicious activity to security.log.
 * 
 * Format: [TIMESTAMP] [SEVERITY_LEVEL] [IP_ADDRESS] [MESSAGE]
 */
void log_suspicious_activity(const char* severity, const char* ip_address, const char* message) {
    // Open file in append mode
    FILE *file = fopen("security.log", "a");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open security.log for writing.\n");
        return;
    }

    // Get current time
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    
    // Format time string
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);

    // Write formatted log entry safely
    fprintf(file, "[%s] [", time_str);
    write_sanitized(file, severity);
    fprintf(file, "] [");
    write_sanitized(file, ip_address);
    fprintf(file, "] ");
    write_sanitized(file, message);
    fprintf(file, "\n");
    
    fclose(file);
}

int main() {
    printf("Logging suspicious activities to security.log...\n");

    // Simulate failed login
    log_suspicious_activity("WARNING", "10.0.0.5", "Failed Login Attempt - User: guest");

    // Simulate access denied
    log_suspicious_activity("CRITICAL", "172.16.0.23", "Access Denied - SQL Injection attempt blocked");

    printf("Done.\n");
    return 0;
}