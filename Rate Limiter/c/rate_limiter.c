#include "rate_limiter.h"
#include <stdio.h>

// Basic hash function (djb2)
static unsigned long hash_ip(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_TABLE_SIZE;
}

/**
 * @brief Initializes the rate limiter state, including the hash table and mutex.
 */
void init_limiter(RateLimiterC* limiter, int max_requests, int window_seconds) {
    limiter->table = (HT_Node**)calloc(HASH_TABLE_SIZE, sizeof(HT_Node*));
    if (limiter->table == NULL) {
        perror("Failed to allocate hash table");
        exit(EXIT_FAILURE);
    }
    limiter->max_requests = max_requests;
    limiter->window_seconds = window_seconds;
    pthread_mutex_init(&limiter->mutex, NULL);
}

/**
 * @brief Frees all memory used by the limiter, including the hash table and nodes.
 */
void free_limiter(RateLimiterC* limiter) {
    pthread_mutex_destroy(&limiter->mutex);
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        HT_Node* current = limiter->table[i];
        while (current != NULL) {
            HT_Node* temp = current;
            current = current->next;
            free(temp->timestamps);
            free(temp);
        }
    }
    free(limiter->table);
}

/**
 * @brief Finds or creates a node for a given IP address in the hash table.
 * This function must be called within a locked mutex.
 */
static HT_Node* find_or_create_node(RateLimiterC* limiter, const char* ip_address) {
    unsigned long index = hash_ip(ip_address);
    HT_Node* current = limiter->table[index];

    while (current != NULL) {
        if (strcmp(current->ip_address, ip_address) == 0) {
            return current;
        }
        current = current->next;
    }

    // IP not found, create a new node
    HT_Node* new_node = (HT_Node*)malloc(sizeof(HT_Node));
    if (new_node == NULL) return NULL;

    strncpy(new_node->ip_address, ip_address, MAX_IP_LEN - 1);
    new_node->ip_address[MAX_IP_LEN - 1] = '\0';
    new_node->count = 0;
    new_node->capacity = 8; // Initial capacity
    new_node->timestamps = (time_t*)malloc(new_node->capacity * sizeof(time_t));
    if (new_node->timestamps == NULL) {
        free(new_node);
        return NULL;
    }
    
    // Add to the front of the linked list at this hash index
    new_node->next = limiter->table[index];
    limiter->table[index] = new_node;
    return new_node;
}

/**
 * @brief Checks if an IP address has exceeded the rate limit.
 *
 * @param limiter The rate limiter instance.
 * @param ip_address The IP address to check.
 * @return 1 if allowed, 0 if blocked.
 */
int check_rate_limit_c(RateLimiterC* limiter, const char* ip_address) {
    pthread_mutex_lock(&limiter->mutex);

    HT_Node* ip_node = find_or_create_node(limiter, ip_address);
    if (ip_node == NULL) {
        pthread_mutex_unlock(&limiter->mutex);
        return 0; // Block if memory allocation fails
    }

    time_t current_time = time(NULL);
    time_t window_start = current_time - limiter->window_seconds;

    // Remove old timestamps (slide the window)
    int valid_index = 0;
    for (int i = 0; i < ip_node->count; ++i) {
        if (ip_node->timestamps[i] > window_start) {
            ip_node->timestamps[valid_index++] = ip_node->timestamps[i];
        }
    }
    ip_node->count = valid_index;

    // Check if limit is exceeded
    if (ip_node->count < limiter->max_requests) {
        // Grow timestamp array if needed
        if (ip_node->count >= ip_node->capacity) {
            ip_node->capacity *= 2;
            time_t* new_ts = (time_t*)realloc(ip_node->timestamps, ip_node->capacity * sizeof(time_t));
            if (new_ts == NULL) {
                // realloc failed, can't record request but don't crash
                pthread_mutex_unlock(&limiter->mutex);
                return 1; // Allow this one time
            }
            ip_node->timestamps = new_ts;
        }
        ip_node->timestamps[ip_node->count++] = current_time;
        pthread_mutex_unlock(&limiter->mutex);
        return 1; // Allowed
    }

    pthread_mutex_unlock(&limiter->mutex);
    return 0; // Blocked
}