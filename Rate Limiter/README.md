# Production-Ready Rate Limiter Implementations

This project provides robust, thread-safe rate limiter implementations in Python, C++, and C. These versions have been upgraded from a simple "Fixed Window" to a more accurate **"Sliding Window Log"** algorithm, making them suitable for production environments.

## Core Concepts

### Sliding Window Log Algorithm
Instead of just counting requests in a static time window, this algorithm stores a timestamp for each request. When a new request comes in, the algorithm:
1.  Removes all timestamps that are older than the current window duration (e.g., the last 60 seconds).
2.  Counts the number of remaining timestamps.
3.  If the count is less than the maximum allowed requests, the new request is accepted and its timestamp is logged. Otherwise, it's rejected.

This provides a much more accurate and fair rate limit than the basic fixed window approach.

### Thread Safety
Modern web servers are multi-threaded. These implementations are now **thread-safe**, meaning multiple server threads can safely access the same rate limiter instance without corrupting its state. This is achieved using `threading.Lock` in Python, `std::mutex` in C++, and `pthread_mutex_t` in C.

### How This Prevents Attacks
A rate limiter is a crucial defense against automated attacks:
*   **Brute Force:** By limiting login attempts per IP, you make guessing passwords computationally infeasible.
*   **Application-Layer DDoS:** By limiting overall requests from any single IP, you can absorb small-scale denial-of-service attacks and bot spam, preserving resources for legitimate users.

---

## Python Implementation

The Python version uses a `defaultdict` to store a list of timestamps for each IP. It is thread-safe and includes a basic cleanup mechanism to prevent unbounded memory growth.

### Quick Start
```bash
python3 python/rate_limiter.py
```

---

## C++ Implementation

The C++ version uses `std::unordered_map` and a `std::vector` of timestamps. It is thread-safe using `std::mutex` and is a high-performance solution for demanding applications.

### Quick Start

1.  Compile the files, making sure to link the pthread library for mutex support.
    ```bash
    g++ -std=c++11 cpp/main.cpp cpp/rate_limiter.cpp -o rate_limiter_cpp -pthread
    ```
2.  Run the executable:
    ```bash
    ./rate_limiter_cpp
    ```

---

## C Implementation

The C version has been significantly upgraded from a linked list to a more performant **hash table**, which provides much faster `O(1)` average-case lookups. It uses a dynamic array for timestamps and is thread-safe using `pthread_mutex_t`.

### Quick Start

1.  Compile the files, linking the pthread library.
    ```bash
    gcc c/main.c c/rate_limiter.c -o rate_limiter_c -pthread
    ```
2.  Run the executable:
    ```bash
    ./rate_limiter_c
    ```

---

## Further Improvements: Persistence with Redis

The current implementations are in-memory, meaning all rate-limiting data is lost on application restart. For a truly distributed and persistent system, you would store the data in an external cache like **Redis**.

**Conceptual Redis Implementation (using ZSET):**

A Redis Sorted Set (`ZSET`) is a perfect data structure for a sliding window rate limiter.

1.  For each IP, create a key (e.g., `ratelimit:<ip_address>`).
2.  When a request arrives, add a new member to the set with the **score and value both set to the current timestamp**.
3.  Remove all members with scores older than the time window (`ZREMRANGEBYSCORE`).
4.  Count the remaining members (`ZCARD`).
5.  If the count is below the limit, the request is allowed. Otherwise, it's denied.

This approach offloads state management to Redis, which is highly optimized for these operations and allows you to share rate-limiting state across many distributed server instances.