import secrets
import hmac

class CSRFManager:
    """
    A class to handle generation and validation of CSRF tokens.
    """

    def __init__(self):
        pass

    def generate_token(self) -> str:
        """
        Generates a cryptographically secure 32-character hexadecimal token.
        
        Returns:
            str: A 32-character hex string representing the token.
        """
        return secrets.token_hex(16)

    def mask_token(self, token: str) -> str:
        """
        Masks the token to prevent BREACH attacks.
        
        The mask is a random string of the same length.
        The result is: mask + (mask XOR token).
        
        Args:
            token (str): The raw 32-character hex token.
            
        Returns:
            str: A 64-character hex string (32-char mask + 32-char masked token).
        """
        # Generate a random mask of the same length (16 bytes = 32 hex chars)
        mask = secrets.token_bytes(16)
        token_bytes = bytes.fromhex(token)
        
        # XOR token with mask
        masked_bytes = bytes(a ^ b for a, b in zip(token_bytes, mask))
        
        # Return mask (hex) + masked_token (hex)
        return mask.hex() + masked_bytes.hex()

    def unmask_token(self, masked_token: str) -> str:
        """
        Unmasks a token received from the client.
        
        Args:
            masked_token (str): The 64-character masked token.
            
        Returns:
            str: The original 32-character hex token, or None if invalid.
        """
        if len(masked_token) != 64:
            return None
            
        try:
            mask = bytes.fromhex(masked_token[:32])
            masked_value = bytes.fromhex(masked_token[32:])
            
            # XOR again to reverse: (token ^ mask) ^ mask = token
            original_bytes = bytes(a ^ b for a, b in zip(masked_value, mask))
            return original_bytes.hex()
        except ValueError:
            return None

    def validate_token(self, received_token: str, stored_token: str) -> bool:
        """
        Validates the received (masked) token against the stored (raw) token.
        
        Args:
            received_token (str): The masked token from the form.
            stored_token (str): The raw token from the session.
            
        Returns:
            bool: True if valid.
        """
        if not isinstance(received_token, str) or not isinstance(stored_token, str):
            return False
            
        unmasked = self.unmask_token(received_token)
        if unmasked is None:
            return False
            
        # hmac.compare_digest is constant-time
        return hmac.compare_digest(unmasked, stored_token)

if __name__ == "__main__":
    # Example usage
    csrf = CSRFManager()
    
    # 1. Generate the persistent session token
    session_token = csrf.generate_token()
    print(f"Session Token (Server-Side): {session_token}")
    
    # 2. Generate a masked token for the HTML form
    # This changes every time, even for the same session token!
    form_token_1 = csrf.mask_token(session_token)
    print(f"Form Token 1 (Client-Side):  {form_token_1}")
    
    form_token_2 = csrf.mask_token(session_token)
    print(f"Form Token 2 (Client-Side):  {form_token_2}")
    
    # 3. Validate
    is_valid = csrf.validate_token(form_token_1, session_token)
    print(f"Validation Result (Token 1): {is_valid}")
    
    is_valid_2 = csrf.validate_token(form_token_2, session_token)
    print(f"Validation Result (Token 2): {is_valid_2}")

    # 4. Attack
    is_valid_attack = csrf.validate_token(form_token_1, "deadbeef" * 4)
    print(f"Validation Result (Attack):  {is_valid_attack}")
