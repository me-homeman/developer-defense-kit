# CORS Configurator

This project demonstrates how to implement strict Cross-Origin Resource Sharing (CORS) enforcement in Python, C++, and C.

## What is CORS?

**Cross-Origin Resource Sharing (CORS)** is a browser security mechanism that restricts HTTP requests initiated from scripts running in a browser to resources in a different origin (domain, protocol, or port).

By default, web browsers enforce the **Same-Origin Policy**, meaning a web page served from `domain-a.com` cannot make an API call to `domain-b.com` unless `domain-b.com` explicitly allows it using specific HTTP headers.

## Security Impact

Without proper CORS configuration, any website could make authorized requests to your API on behalf of a logged-in user.

-   **Loose Configuration (Wildcard `*`):** Allowing all origins (`Access-Control-Allow-Origin: *`) lets any malicious site access your API, potentially reading private data if authentication tokens (like cookies) are sent automatically.
-   **Strict Configuration (This Project):** By checking the `Origin` header and only allowing a specific trusted domain (e.g., `https://yourfrontend.com`), we ensure that only our own frontend can interact with the backend API.

## Setup Guide

Place the files in their respective directories as shown below:

-   **Python:** `python/cors_middleware.py` (Requires Flask: `pip install flask`)
-   **C++:** `cpp/cors_filter.cpp`
-   **C:** `c/cors_headers.c`

## Compilation and Running

### C++
To compile the C++ implementation:
```bash
g++ cpp/cors_filter.cpp -o cpp/cors_filter
./cpp/cors_filter
```

### C
To compile the C implementation:
```bash
gcc c/cors_headers.c -o c/cors_headers
./c/cors_headers
```

### Python
To run the Python Flask middleware:
```bash
python3 python/cors_middleware.py
```

## Project Structure

### `python/cors_middleware.py`
A Flask-based middleware that intercepts requests, checks the `Origin` header, and either aborts with 403 or injects the appropriate CORS headers.

### `cpp/cors_filter.cpp`
A C++ class `CORSFilter` using `std::unordered_map` to manage request and response headers, implementing the validation logic.

### `c/cors_headers.c`
A C implementation demonstrating low-level string manipulation with `strcmp` and `snprintf` to validate the origin and construct the raw HTTP response headers.
