# Secure Environment Variable Loader

This project provides utilities in **Python**, **C++**, and **C** to securely load configuration settings and secrets (like API keys and database passwords) from environment variables or `.env` files.

## 1. The Why: Danger of Credential Leakage

Hardcoding secrets (e.g., `const string API_KEY = "12345-secret";`) in source code is a critical security vulnerability.
- **Version Control Risks**: If you commit this code to Git, the secret becomes permanent in the history. Anyone with access to the repo (or if it's public) can steal your credentials.
- **Deployment inflexibility**: Hardcoded values require recompilation to change configurations between Development, Staging, and Production.

**Solution**: Use **Environment Variables**. They keep secrets in memory or local files that are *never* committed to version control.

## 2. Setup

### 1. Create a `.env` file
Create a file named `.env` in the root of the project directory. Add your secrets there:
```ini
API_KEY=super_secret_api_key_123
DB_PASSWORD=my_secure_db_password
```

### 2. Protect with `.gitignore`
Create a `.gitignore` file to ensure the `.env` file is **never** committed:
```text
.env
__pycache__/
*.o
*.exe
*.out
```

## 3. Implementation Details & Usage

All implementations follow these rules:
1.  **Priority**: Check system environment variables first (for Docker/CI). If missing, check `.env`.
2.  **Fail Fast**: Exit immediately if a critical key is missing.
3.  **Safety Check**: Refuse to start if the value is a placeholder like "REPLACE_ME".

### Python
Requires `python-dotenv`.
```bash
pip install python-dotenv
python3 config_loader.py
```

### C++
Compile and run:
```bash
g++ -o config_loader_cpp config_loader.cpp
./config_loader_cpp
```

### C
Compile and run:
```bash
gcc -o config_loader_c config_loader.c
./config_loader_c
```

## 4. Best Practices
- **Rotate Secrets**: Change your API keys regularly.
- **Least Privilege**: Give your API keys only the permissions they need.
- **Use Secret Managers**: In complex production environments (AWS, Azure), use dedicated services like AWS Secrets Manager or HashiCorp Vault instead of `.env` files.
