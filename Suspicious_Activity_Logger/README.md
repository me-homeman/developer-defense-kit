# Suspicious Activity Logger

## Overview
This project provides a standardized logging utility implemented in Python, C++, and C. Its primary purpose is to monitor and record suspicious activities such as "Failed Login Attempts" and "Access Denied" errors. By logging these events in a consistent format, administrators can detect potential threats, such as Brute Force attacks or unauthorized scanning, early in the attack lifecycle.

## Security Hardening (New!)
**Vulnerability Fixed: Log Injection (CRLF Injection)**
An audit (Test Level: Hard) identified that the original implementation was vulnerable to Log Injection attacks. Attackers could insert newline characters (`\n`) into the input fields to forge malicious log entries, potentially confusing SIEM systems or administrators.

**The Fix:**
All implementations (Python, C, C++) now include **Input Sanitization**.
- Newlines (`\n`, `\r`) are escaped to `\n` and `\r`.
- Control characters are replaced or stripped.
- This ensures that one event equals exactly one line in the log file.

## Detection Logic
The generated `security.log` file is designed to be parsed easily by humans or automated SIEM tools.
- **Silent Failures:** Look for repeated `WARNING` level logs from the same `IP_ADDRESS` within a short timeframe.
- **Scanning Behavior:** Sequential access denied errors across different endpoints (or message contents) from a single IP indicate a vulnerability scan.
- **Brute Force:** High frequency of "Failed Login Attempt" messages from a single IP address.

## Setup Guide

### Directory Structure
Ensure your project follows this structure:
```
Suspicious_Activity_Logger/
├── python/
│   └── logger.py
├── cpp/
│   └── Logger.cpp
├── c/
│   └── security_log.c
└── README.md
```

### Python
The Python implementation is located in `python/logger.py`. It uses the standard `logging` library with a custom wrapper for sanitization.
To run:
```bash
python3 python/logger.py
```

## Compilation

### C++
The C++ implementation is located in `cpp/Logger.cpp`.
To compile and run:
```bash
g++ cpp/Logger.cpp -o cpp_logger
./cpp_logger
```

### C
The C implementation is located in `c/security_log.c`.
To compile and run:
```bash
gcc c/security_log.c -o c_logger
./c_logger
```

## Maintenance
**Log Rotation:** The `security.log` file will grow indefinitely as new events are appended. It is highly recommended to implement log rotation (e.g., using `logrotate` on Linux) to archive old logs and prevent disk space exhaustion. A simple daily rotation policy keeping 7 days of logs is usually sufficient for small to medium deployments.