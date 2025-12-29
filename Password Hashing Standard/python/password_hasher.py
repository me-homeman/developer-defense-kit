import argon2
from argon2 import PasswordHasher, Type
from argon2.exceptions import VerifyMismatchError

class PasswordUtility:
    def __init__(self, time_cost=3, memory_cost=65536, parallelism=4):
        """
        Initialize the hasher with specific parameters.
        Default parameters are safe for general use.
        """
        self.ph = PasswordHasher(
            time_cost=time_cost,
            memory_cost=memory_cost,
            parallelism=parallelism,
            type=Type.ID
        )

    def hash_password(self, password: str) -> str:
        """
        Hashes a password using Argon2id with automatic salting.
        """
        return self.ph.hash(password)

    def check_password(self, hash_str: str, password: str) -> bool:
        """
        Verifies a password against a hash.
        """
        try:
            return self.ph.verify(hash_str, password)
        except VerifyMismatchError:
            return False
        except Exception as e:
            print(f"Error checking password: {e}")
            return False

    def check_needs_rehash(self, hash_str: str) -> bool:
        """
        Checks if a hash needs to be regenerated (e.g., if parameters have changed).
        Call this AFTER a successful check_password.
        """
        return self.ph.check_needs_rehash(hash_str)

def main():
    print("--- Python Argon2id Password Utility (Production Ready) ---")
    
    # 1. Simulate an OLD system with weaker parameters
    print("\n[Step 1] Creating hash with 'Legacy' parameters (Cost: 1)...")
    legacy_tool = PasswordUtility(time_cost=1, memory_cost=8192)
    password = "MySecurePassword123!"
    legacy_hash = legacy_tool.hash_password(password)
    print(f"Legacy Hash: {legacy_hash}")

    # 2. Simulate the NEW system with stronger parameters (Cost: 3)
    print("\n[Step 2] System upgraded. Verifying with 'Modern' parameters...")
    modern_tool = PasswordUtility(time_cost=3, memory_cost=65536)

    # Verify password (works even with old parameters)
    if modern_tool.check_password(legacy_hash, password):
        print("Password verified successfully.")
        
        # 3. Check if we need to upgrade the hash
        if modern_tool.check_needs_rehash(legacy_hash):
            print("[!] Hash uses old parameters. Upgrading hash...")
            new_hash = modern_tool.hash_password(password)
            print(f"New Hash:    {new_hash}")
            print("Action: Update this new hash in the database.")
        else:
            print("Hash is up to date.")
    else:
        print("Invalid password.")

if __name__ == "__main__":
    main()