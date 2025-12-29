# 🛡️ developer-defense-kit

**"Don't reinvent the wheel. Just secure it."**

This repository contains copy-paste ready code snippets to patch common security vulnerabilities in web applications. These scripts are designed to be dropped into new projects (Python, Node.js, PHP) to prevent low-hanging fruit attacks.

## 📂 Project Structure & Roadmap

Below are the 10 essential security modules this repository aims to provide.

### 1. 💉 Anti-SQL Injection Wrapper
- **Goal:** A wrapper function that forces the use of Parameterized Queries.
- **Prevents:** SQL Injection (SQLi) where attackers dump or delete your database.

### 2. 🛑 The "Rate Limiter" Middleware
- **File:** `/firewall/request_limiter`
- **Goal:** Tracks IP addresses and blocks them if they exceed X requests per minute.
- **Prevents:** Brute Force attacks on login pages and simple DDoS spam.

### 3. 🧹 Input Sanitizer (XSS Filter)
- **Goal:** Strips dangerous HTML tags (`<script>`, `<iframe>`) from user input.
- **Prevents:** Stored Cross-Site Scripting (XSS) where hackers run JS in users' browsers.

### 4. 🔒 Password Hashing Standard
- **Goal:** A utility that solely uses `bcrypt` or `Argon2` (no MD5/SHA1). Includes salting automatically.
- **Prevents:** Rainbow table attacks if your database is leaked.

### 5. 📑 Secure Headers Injector
- **Goal:** Sets `Content-Security-Policy`, `X-Frame-Options`, and `HSTS` on every response.
- **Prevents:** Clickjacking and Man-in-the-Middle downgrades.

### 6. 📁 File Upload Validator
- **Goal:** Checks "Magic Numbers" (file signatures) to prove a file is actually a JPG/PDF, not just renamed.
- **Prevents:** Malicious Shell Uploads (e.g., `hack.php.jpg`).

### 7. 🔑 Environment Variable Loader
- **Goal:** Ensures API keys and DB passwords are loaded from `.env` and crashes the app if hardcoded secrets are detected.
- **Prevents:** Credential leakage on GitHub.

### 8. 🌐 CORS Configurator
- **Goal:** Strict Cross-Origin Resource Sharing settings. Only allows requests from *your* frontend domain.
- **Prevents:** Unauthorized API calls from malicious websites.

### 9. 🎫 CSRF Token Generator
- **Goal:** Generates and validates unique tokens for form submissions.
- **Prevents:** Cross-Site Request Forgery (tricking users into clicking buttons they didn't mean to).

### 10. 🕵️ Suspicious Activity Logger
- **Goal:** Specifically logs "Failed Login Attempts" and "Access Denied" errors to a separate file.
- **Prevents:** Silent failures (lets you see if someone is scanning you).

---

## ⚠️ Disclaimer
These snippets are for educational and hardening purposes. Security is a continuous process. Always keep your dependencies updated.

## 🤝 Contribution
Found a better way to patch a vulnerability? Submit a Pull Request.
