# Simple XSS Filter Utility (v2.0)

## Overview

This project provides an **Input Sanitizer (XSS Filter)** implementation in Python, C++, and C. 

**Version 2.0 Update:** The filter has been significantly enhanced to follow security best practices, moving beyond simple tag stripping.

**Features:**
*   **Python:** Uses `html.parser` for a **Whitelist-based** approach (most secure). Allows only specific tags (`<b>`, `<i>`, etc.) and strips everything else.
*   **C++ / C:** Implements **Enhanced Blacklisting** (Tag stripping + Event Handler stripping + Protocol stripping) and **Output Encoding**.
*   **Defense in Depth:** All implementations now demonstrate **Output Encoding** (HTML Entity Encoding), which is the most effective defense against XSS.

## How to Use

Each directory contains a standalone implementation with a `main()` function demonstrating both **Sanitization** (cleaning input) and **Output Encoding** (safe display).

### Python (Recommended)
Uses the standard library `html.parser` to enforce a strict whitelist.

**Run:**
```bash
python3 python/xss_filter.py
```

### C++
Uses modern C++ `std::regex` to identify and neutralize tags, `on*` attributes, and `javascript:` URIs.

**Compile & Run:**
```bash
g++ cpp/xss_filter.cpp -o cpp/xss_filter
./cpp/xss_filter
```

### C
Demonstrates low-level string parsing to mask dangerous patterns in-place.

**Compile & Run:**
```bash
gcc c/xss_filter.c -o c/xss_filter
./c/xss_filter
```

## Compilation Guide

Ensure you have `gcc` and `g++` installed.

**For C++ (Standard C++11 or later):**
```bash
g++ -std=c++11 cpp/xss_filter.cpp -o cpp_filter
```

**For C:**
```bash
gcc c/xss_filter.c -o c_filter
```

## Security Disclaimer

**v2.0 Improvements:**
1.  **Whitelisting (Python):** "Allow only known good" is safer than "Block known bad".
2.  **Attribute Stripping:** Removes `onload`, `onerror`, `onclick`, etc.
3.  **Protocol Validation:** Blocks `javascript:alert(1)` links.
4.  **Output Encoding:** Converts `<` to `&lt;`, rendering scripts harmless in the browser.

**Note:** While significantly improved, the C and C++ regex/string-based filters are simulations. For production C/C++ applications, link against a battle-tested library like **Google Gumbo** or **libxml2** for true HTML parsing security.
