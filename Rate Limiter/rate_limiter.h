#ifndef RATE_LIMITER_C_H
#define RATE_LIMITER_C_H

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define MAX_IP_LEN 16
#define HASH_TABLE_SIZE 1024

// Node for the hash table, stores request timestamps for an IP
typedef struct HT_Node {
    char ip_address[MAX_IP_LEN];
    time_t* timestamps;
    int count; // Number of timestamps currently stored
    int capacity; // Allocated capacity of the timestamps array
    struct HT_Node* next; // For handling hash collisions
} HT_Node;

// Rate limiter state
typedef struct {
    HT_Node** table; // Hash table
    int max_requests;
    int window_seconds;
    pthread_mutex_t mutex; // Mutex for thread safety
} RateLimiterC;

// Public functions
void init_limiter(RateLimiterC* limiter, int max_requests, int window_seconds);
int check_rate_limit_c(RateLimiterC* limiter, const char* ip_address);
void free_limiter(RateLimiterC* limiter);

#endif // RATE_LIMITER_C_H