# Secure Headers Injector

A collection of utilities in Python, C++, and C designed to harden web applications by automatically injecting essential OWASP-recommended security headers into HTTP responses.

## Introduction

Modern web applications face numerous client-side threats. Without proper configuration, users are vulnerable to attacks that exploit browser behaviors.

*   **Clickjacking:** Attackers embed your site in an invisible iframe to trick users into clicking buttons (e.g., "Delete Account") they didn't intend to.
*   **MIME Sniffing:** Browsers may incorrectly interpret a file as executable code (e.g., a text file as JavaScript), leading to Cross-Site Scripting (XSS).
*   **Protocol Downgrade Attacks:** Attackers intercept traffic to strip SSL/TLS protection, forcing users onto insecure HTTP connections.

This utility mitigates these risks by enforcing a standard set of "Secure Headers" at the middleware or response level.

## Header Breakdown

The following headers are injected by this utility:

| Header | Value | Purpose |
| :--- | :--- | :--- |
| **Content-Security-Policy (CSP)** | `default-src 'self'` | Restricts resources (scripts, images, styles) to load only from the same origin, drastically reducing XSS and data injection risks. |
| **X-Frame-Options** | `DENY` | Prevents the page from being displayed in a frame/iframe. This completely stops **Clickjacking** attacks. |
| **Strict-Transport-Security (HSTS)** | `max-age=31536000; includeSubDomains` | Tells the browser to *only* access the site via HTTPS for the next year, preventing **Man-in-the-Middle (MitM)** downgrade attacks. |
| **X-Content-Type-Options** | `nosniff` | Forces the browser to strictly follow the declared `Content-Type`, preventing **MIME type sniffing** security flaws. |

## Usage Guide

### Python
Designed for frameworks like Flask, FastAPI, or Django.
1.  Import `SecureHeaderMiddleware`.
2.  Call `inject_headers(response.headers)` before sending the response.

### C++
Designed for modern C++ web servers (e.g., Crow, Drogon).
1.  Include the `SecureHeadersInjector` class.
2.  Pass your response header map to `SecureHeadersInjector::inject()`.

### C
Designed for embedded systems or raw socket servers.
1.  Ensure your response buffer has sufficient space.
2.  Call `inject_secure_headers(buffer, max_size)` to append the headers directly to the response string.

## Compilation & Testing

To compile and run the provided mock examples:

**C++**
```bash
g++ cpp/secure_headers.cpp -o secure_headers_cpp
./secure_headers_cpp
```

**C**
```bash
gcc c/secure_headers.c -o secure_headers_c
./secure_headers_c
```

**Python**
```bash
python3 python/secure_headers.py
```
