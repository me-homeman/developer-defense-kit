#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

// Pre-formatted string of security headers to append
const char* SECURITY_HEADERS = 
    "Content-Security-Policy: default-src 'self'\r\n"
    "X-Frame-Options: DENY\r\n"
    "Strict-Transport-Security: max-age=31536000; includeSubDomains\r\n"
    "X-Content-Type-Options: nosniff\r\n";

/**
 * Appends security headers to a raw HTTP response buffer.
 * This function performs low-level string manipulation suitable for 
 * embedded servers or raw socket programming.
 * 
 * @param buffer The mutable character buffer containing the HTTP response headers.
 * @param max_size The total allocated size of the buffer to prevent overflow.
 * @return 0 on success, -1 if buffer overflow would occur.
 */
int inject_secure_headers(char* buffer, size_t max_size) {
    size_t current_len = strlen(buffer);
    size_t headers_len = strlen(SECURITY_HEADERS);

    // Check for buffer overflow
    if (current_len + headers_len >= max_size) {
        return -1; 
    }

    // Check if the headers are already terminated with double CRLF
    char* body_separator = strstr(buffer, "\r\n\r\n");

    if (body_separator != NULL) {
        // Headers are terminated. We must insert BEFORE the final empty line.
        // body_separator points to the first \r of the \r\n\r\n sequence.
        // We want to keep the first \r\n (terminating the previous header).
        // So we insert at body_separator + 2.
        
        char* insertion_point = body_separator + 2;
        size_t tail_len = strlen(insertion_point);
        
        // Check for buffer overflow with the new offset logic (just to be safe, 
        // though strictly handled by initial check since we are just shifting)
        // Actually, logic is: current_len + headers_len < max_size. Checked above.

        // Move the tail (the final \r\n and body) to the right
        memmove(insertion_point + headers_len, insertion_point, tail_len + 1);
        
        // Copy the new headers into the gap
        memcpy(insertion_point, SECURITY_HEADERS, headers_len);
        
    } else {
        // No double CRLF found. Assume headers are being built and append to end.
        strcat(buffer, SECURITY_HEADERS);
    }

    return 0;
}

// --- Mock Usage / Test Case ---
#ifdef BUILD_MAIN
int main() {
    printf("--- C Secure Headers Injector Demo ---\n");

    // Simulate a raw HTTP response buffer
    // In a real server, this would be populated during request processing
    char response_buffer[BUFFER_SIZE] = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Server: MyCServer/0.1\r\n";

    printf("Original Response Buffer:\n%s\n", response_buffer);

    printf("[+] Injecting headers...\n");
    if (inject_secure_headers(response_buffer, BUFFER_SIZE) == 0) {
        printf("\nFinal Response Buffer:\n%s\n", response_buffer);
    } else {
        fprintf(stderr, "Error: Buffer too small to inject headers.\n");
        return 1;
    }

    return 0;
}
#endif
