# File Upload Validator (Magic Number Check)

## Concept
One of the most common security vulnerabilities in web applications is Unrestricted File Upload. Attackers often attempt to bypass security checks by simply renaming a malicious file (e.g., `exploit.php`) to a harmless extension (e.g., `exploit.php.jpg`).

If the backend only validates the **file extension**, the server might accept the file. However, if the server (or another user) executes it, the attacker gains code execution.

**The Solution:** Validate the **content** of the file. Every file type has a unique "signature" or "Magic Number" at the very beginning of the binary data. By checking these bytes, we can verify the file's true identity, regardless of its extension.

## Magic Numbers
This utility checks for the following hex signatures:

| File Type | Hex Signature | ASCII Representation |
|-----------|---------------|----------------------|
| **JPG**   | `FF D8 FF`    | `ÿØÿ`                |
| **PDF**   | `25 50 44 46` | `%PDF`               |

## Usage Guide

### 1. Python
The Python script uses the built-in `open()` and `read()` methods for a lightweight solution.
```bash
# Navigate to the python directory
cd python

# Run the script
python3 validator.py
```

### 2. C++
The C++ implementation uses `std::ifstream` for binary reading.
```bash
# Navigate to the cpp directory
cd cpp

# Compile
g++ -o validator validator.cpp

# Run
./validator
```

### 3. C
The C implementation uses standard `fopen`, `fread`, and `memcmp` for low-level validation.
```bash
# Navigate to the c directory
cd c

# Compile
gcc -o validator validator.c

# Run
./validator
```

## Compilation Details
*   **C++:** Tested with `g++` (Standard C++11 or later recommended).
*   **C:** Tested with `gcc` (ANSI C or C99 compliant).

## Future Hardening
To further secure file uploads, consider implementing:
1.  **MIME Type Checking:** Use libraries (like `libmagic` on Linux or `python-magic`) to check the MIME type, though this is sometimes just a lookup based on extension or magic numbers.
2.  **Full Content Scanning:** Magic numbers only check the header. A file could have a valid header but malicious content appended (e.g., Polyglot files).
3.  **Antivirus Scanning:** Integrate with tools like ClamAV to scan uploaded files for known malware signatures.
4.  **Image Re-processing:** For images, re-encoding them (e.g., using ImageMagick or PIL) can strip out malicious metadata or injected code.
