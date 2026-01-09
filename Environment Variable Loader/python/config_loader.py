import os
import sys
from dotenv import load_dotenv

class ConfigLoader:
    def __init__(self):
        # Load .env file variables into environment
        # override=False ensures system env vars take precedence over .env file
        load_dotenv(override=False)

    def get_secret(self, key: str) -> str:
        """
        Retrieves a secret from the environment.
        Fails fast if missing.
        Performs safety checks against placeholders.
        """
        value = os.getenv(key)

        # 1. Fail Fast
        if value is None:
            print(f"CRITICAL ERROR: Missing required environment variable: '{key}'")
            sys.exit(1)

        # 2. Safety Check (Hardcode Prevention)
        unsafe_placeholders = ["REPLACE_ME", "YOUR_PASSWORD_HERE", "CHANGE_THIS"]
        if value in unsafe_placeholders:
            print(f"SECURITY ALERT: Default placeholder detected for '{key}'. Update your configuration.")
            sys.exit(1)

        return value

def mask_secret(secret: str) -> str:
    """Helper to mask secret for display purposes."""
    if len(secret) <= 4:
        return "****"
    return secret[:2] + "*" * (len(secret) - 4) + secret[-2:]

if __name__ == "__main__":
    loader = ConfigLoader()
    
    # Try to load a variable named 'API_KEY'
    # Note: Ensure you have a .env file or export API_KEY before running
    target_key = "API_KEY"
    
    print(f"Attempting to load {target_key}...")
    secret_value = loader.get_secret(target_key)
    
    print(f"Success! Securely loaded {target_key}: {mask_secret(secret_value)}")
