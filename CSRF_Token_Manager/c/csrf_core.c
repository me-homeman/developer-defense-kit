#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to generate a 32-character hex token
char* generate_token() {
    FILE *fp = fopen("/dev/urandom", "r");
    if (!fp) {
        perror("Failed to open /dev/urandom");
        return NULL;
    }

    unsigned char buffer[16];
    if (fread(buffer, 1, 16, fp) != 16) {
        perror("Failed to read random bytes");
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    char *token = (char *)malloc(33);
    if (!token) {
        perror("Failed to allocate memory");
        return NULL;
    }

    for (int i = 0; i < 16; i++) {
        sprintf(token + (i * 2), "%02x", buffer[i]);
    }
    token[32] = '\0';

    return token;
}

// Function to validate token using constant-time comparison
int validate_token(const char *received_token, const char *stored_token) {
    if (received_token == NULL || stored_token == NULL) {
        return 0;
    }

    // Volatile to prevent compiler optimization of the loop
    volatile unsigned char result = 0;
    
    // We assume standard token length of 32 for our specific application
    // If lengths differ, we flag it but SHOULD try to run the loop to normalize timing
    // However, safely reading past the end of a string in C is a buffer overread risk.
    // So we check length, but if they match, we ensure the loop runs fully.
    
    size_t len_r = strlen(received_token);
    size_t len_s = strlen(stored_token);

    if (len_r != 32 || len_s != 32) {
        return 0; // Protocol violation
    }

    for (size_t i = 0; i < 32; i++) {
        result |= (received_token[i] ^ stored_token[i]);
    }

    return (result == 0);
}

int main() {
    char *session_token = generate_token();
    if (!session_token) {
        return 1;
    }
    printf("Generated Token: %s\n", session_token);

    if (validate_token(session_token, session_token)) {
        printf("Validation Result (Valid): Success\n");
    } else {
        printf("Validation Result (Valid): Failure\n");
    }

    if (validate_token(session_token, "wrongtokenwrongtokenwrongtoken!!")) {
        printf("Validation Result (Invalid): Success\n");
    } else {
        printf("Validation Result (Invalid): Failure\n");
    }

    free(session_token);
    return 0;
}