# Password Hashing Utility (Production Hardened)

This repository contains **production-grade** password hashing implementations in **Python**, **C++**, and **C**. It goes beyond basic hashing to include memory safety protections, automatic hash upgrading (rehashing), and secure configuration defaults.

**Algorithm:** Argon2id (Winner of the Password Hashing Competition)
**Libraries:** `libsodium` (C/C++), `argon2-cffi` (Python)

## 1. Advanced Security Features

### A. Automatic Hash Upgrades (Rehashing)
Security standards change. What is "secure" today (e.g., Cost 3) might be "weak" in 5 years.
*   **The Feature:** All implementations include logic to check if a user's hash was created with old parameters (`check_needs_rehash`).
*   **The Flow:** When a user logs in successfully, the system checks if their hash is "old". If yes, it transparently re-hashes the password with *current* parameters and updates the database.

### B. Memory Safety (C/C++)
Handling plaintext passwords is dangerous. If the application crashes (core dump) or runs out of RAM (swap), passwords can be written to disk in plain text.
*   **C++:** Uses a custom `SodiumAllocator` to create a `SecureString` type. This allocates memory using `sodium_alloc` (guarded pages) and automatically calls `sodium_free` (wipe/zero-fill) when the string goes out of scope.
*   **C:** Uses `sodium_mlock` to "lock" the password buffer in RAM, preventing the OS from swapping it to disk, and `sodium_munlock` to securely wipe it after use.

## 2. Installation Guide

### Linux (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install build-essential libsodium-dev python3-pip
pip3 install argon2-cffi
```

### macOS (Homebrew)
```bash
brew install libsodium
pip3 install argon2-cffi
```

## 3. Compilation & Usage

### Python (Rehashing Demo)
Demonstrates upgrading a "weak" legacy hash to a modern secure hash.
```bash
python3 python/password_hasher.py
```

### C++ (Secure Allocator)
Requires C++17. Demonstrates `SecureString` which auto-wipes memory.
```bash
# Compile
g++ -std=c++17 cpp/password_hasher.cpp -o cpp/hasher_cpp -lsodium

# Run
./cpp/hasher_cpp
```

### C (Locked Memory)
Demonstrates `sodium_mlock` to prevent disk swapping.
```bash
# Compile
gcc c/password_hasher.c -o c/hasher_c -lsodium

# Run
./c/hasher_c
```

## 4. Production Checklist

1.  **Tune Work Factors:** The default `interactive` limits are safe defaults. For high-security backend systems, tune the `OPSLIMIT` and `MEMLIMIT` to the maximum values your server can handle while keeping login times under 500ms.
2.  **Peppering (Optional):** Consider adding a server-side secret "pepper" key to passwords before hashing.
3.  **Strict Inputs:** Ensure strict input sanitization to prevent buffer overflows before the data even reaches the hasher.
