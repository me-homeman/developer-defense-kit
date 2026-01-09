#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ALLOWED_ORIGIN "https://yourfrontend.com"
#define MAX_HEADER_SIZE 1024

// Structure to represent a simple HTTP response
typedef struct {
    int status_code;
    char headers[MAX_HEADER_SIZE];
} HttpResponse;

// Function to apply CORS logic
// Returns 1 if allowed, 0 if blocked
int apply_cors(const char* request_origin, HttpResponse* response) {
    // Reset headers
    response->headers[0] = '\0';

    if (request_origin == NULL) {
        // No Origin header, typically safe to proceed (same-origin)
        return 1;
    }

    if (strcmp(request_origin, ALLOWED_ORIGIN) == 0) {
        // Match found, construct headers
        snprintf(response->headers, MAX_HEADER_SIZE,
            "Access-Control-Allow-Origin: %s\r\n"
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
            "Vary: Origin\r\n",
            ALLOWED_ORIGIN);
        return 1;
    } else {
        // Mismatch, block request
        response->status_code = 403;
        return 0;
    }
}

int main() {
    HttpResponse response;
    
    // Test Case 1: Valid Origin
    printf("Test 1: Origin: %s\n", ALLOWED_ORIGIN);
    response.status_code = 200;
    if (apply_cors(ALLOWED_ORIGIN, &response)) {
        printf("Result: Allowed\nHeaders Injected:\n%s", response.headers);
    } else {
        printf("Result: Blocked\n");
    }

    printf("\n--------------------------------\n\n");

    // Test Case 2: Invalid Origin
    const char* bad_origin = "https://evil.com";
    printf("Test 2: Origin: %s\n", bad_origin);
    response.status_code = 200;
    if (apply_cors(bad_origin, &response)) {
        printf("Result: Allowed\n");
    } else {
        printf("Result: Blocked with Status Code: %d\n", response.status_code);
    }

    return 0;
}
