# CSRF Token Manager

This project implements a Cross-Site Request Forgery (CSRF) protection mechanism in Python, C++, and C. It demonstrates how to generate cryptographically secure tokens and validate them using constant-time comparison to prevent timing attacks.

## Security Audit & Vulnerability Fixes (Hardened)

A rigorous security audit was performed on the initial implementation. The following vulnerabilities were identified and fixed:

1.  **Insecure RNG (C++):** The use of `std::mt19937` was removed. It is not a CSPRNG. The implementation now directly reads from `/dev/urandom` to ensure unpredictable tokens.
2.  **BREACH Attack Vulnerability (Python):** The initial version used static tokens. **Token Masking** was added (`mask_token`, `unmask_token`) to XOR the session token with a random mask on every request. This mitigates compression-based side-channel attacks.
3.  **Timing Leaks (C/C++):** Validation logic was hardened to prevent compiler optimizations from creating timing side-channels during string comparison.

## CSRF Explained

Cross-Site Request Forgery (CSRF) is a vulnerability where an attacker tricks an authenticated user into performing unwanted actions on a web application.

## The Solution: Synchronizer Token Pattern (STP)

### Token Masking (New!)
To prevent **BREACH** attacks, the raw session token is never sent directly in the HTML form. Instead, we send:
`masked_token = random_mask + (random_mask XOR session_token)`

The server reverses this process to validate. This ensures the token string in the HTML changes on every request, rendering compression side-channels useless.

## Deployment Guide

```
CSRF_Token_Manager/
├── python/
│   └── csrf_util.py      # Implements Token Masking + HMAC
├── cpp/
│   └── CsrfToken.cpp     # Hardened /dev/urandom reader + constant-time check
└── c/
    └── csrf_core.c       # Low-level secure memory and comparison
```

### Python
The `CSRFManager` class now supports masking.
- **`generate_token()`**: Creates the persistent session secret.
- **`mask_token(token)`**: Creates a one-time use token for the HTML form.
- **`validate_token(form_token, session_token)`**: Unmasks and validates.

### C++
Now requires a POSIX environment (Linux/macOS) to access `/dev/urandom`.

### C
Strict memory management and constant-time comparisons.

## Compilation

To compile and run the provided examples, use the following commands:

### C++ (g++)
```bash
g++ -std=c++17 -o csrf_cpp cpp/CsrfToken.cpp
./csrf_cpp
```

### C (gcc)
```bash
gcc -o csrf_c c/csrf_core.c
./csrf_c
```

### Python
```bash
python3 python/csrf_util.py
```

## Security Note

**Constant-Time Comparison:**
This implementation uses constant-time string comparison. This is critical to prevent **Timing Attacks**, where an attacker could deduce the token byte-by-byte by measuring how long the server takes to reject an incorrect token.