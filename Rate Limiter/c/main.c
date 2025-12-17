#include "rate_limiter.h"
#include <stdio.h>
#include <unistd.h> // for sleep()
#include <pthread.h>

// Shared limiter for all threads
RateLimiterC limiter;

typedef struct {
    const char* ip;
    int thread_id;
} thread_arg_t;

void* make_request_thread(void* arg) {
    thread_arg_t* thread_arg = (thread_arg_t*)arg;
    if (check_rate_limit_c(&limiter, thread_arg->ip)) {
        printf("[Thread %d] Request from %s ALLOWED.\n", thread_arg->thread_id, thread_arg->ip);
    } else {
        printf("[Thread %d] Request from %s BLOCKED.\n", thread_arg->thread_id, thread_arg->ip);
    }
    return NULL;
}

int main() {
    // 5 requests per 10 seconds
    init_limiter(&limiter, 5, 10);

    printf("--- Simulating concurrent requests from 192.168.1.1 ---\n");
    
    pthread_t threads[7];
    thread_arg_t args[7];

    for (int i = 0; i < 7; ++i) {
        args[i] = (thread_arg_t){"192.168.1.1", i + 1};
        pthread_create(&threads[i], NULL, make_request_thread, &args[i]);
        usleep(100 * 1000); // 100ms
    }

    for (int i = 0; i < 7; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--- Waiting for 11 seconds... \n\n"); // Increased sleep time
    sleep(11);

    printf("--- Making more requests, some should now be allowed ---\n");
    pthread_t more_threads[5];
    thread_arg_t more_args[5];
    for (int i = 0; i < 5; ++i) {
        more_args[i] = (thread_arg_t){"192.168.1.1", i + 8};
        pthread_create(&more_threads[i], NULL, make_request_thread, &more_args[i]);
        sleep(1);
    }

    for (int i = 0; i < 5; ++i) {
        pthread_join(more_threads[i], NULL);
    }

    free_limiter(&limiter);
    return 0;
}
