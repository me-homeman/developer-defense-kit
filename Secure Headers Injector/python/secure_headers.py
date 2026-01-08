import http.server
import socketserver

class SecureHeaderMiddleware:
    """
    Middleware utility to inject secure headers into HTTP responses.
    Designed to be compatible with dictionary-like header objects used in 
    frameworks like Flask, Django, or FastAPI.
    """
    
    SECURITY_HEADERS = {
        "Content-Security-Policy": "default-src 'self'",
        "X-Frame-Options": "DENY",
        "Strict-Transport-Security": "max-age=31536000; includeSubDomains",
        "X-Content-Type-Options": "nosniff"
    }

    @staticmethod
    def inject_headers(response_headers):
        """
        Injects security headers into a dictionary-like headers object.
        
        Args:
            response_headers (dict): The mutable headers object from the response.
        """
        # First, remove any existing security headers to prevent duplicates/conflicts,
        # handling case-insensitivity (e.g., remove 'content-security-policy' 
        # before adding 'Content-Security-Policy').
        existing_keys = list(response_headers.keys())
        for existing_key in existing_keys:
            for secure_key in SecureHeaderMiddleware.SECURITY_HEADERS.keys():
                if existing_key.lower() == secure_key.lower():
                    del response_headers[existing_key]

        # Inject the secure defaults
        for key, value in SecureHeaderMiddleware.SECURITY_HEADERS.items():
            response_headers[key] = value

# --- Mock Usage / Test Case ---
def mock_run():
    print("--- Python Secure Headers Injector Demo ---")
    
    # Scenario: A Flask-like response object with a headers dictionary
    response_headers = {
        "Content-Type": "text/html; charset=utf-8",
        "Server": "MyAppServer/2.1"
    }
    
    print("Original Headers:")
    for k, v in response_headers.items():
        print(f"  {k}: {v}")
        
    print("\n[+] Injecting Security Headers...")
    SecureHeaderMiddleware.inject_headers(response_headers)
    
    print("\nFinal Headers:")
    for k, v in response_headers.items():
        print(f"  {k}: {v}")

if __name__ == "__main__":
    mock_run()
