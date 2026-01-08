import os
import sys

class HardenedFileValidator:
    """
    Level 2 Security: Checks Magic Numbers AND scans content for suspicious strings.
    """
    
    SIGNATURE_JPG = b'\xFF\xD8\xFF'
    SIGNATURE_PDF = b'\x25\x50\x44\x46'
    
    # Suspicious signatures (simple example: PHP tags)
    BLACKLIST_SIGNATURES = [
        b'<?php', 
        b'eval(', 
        b'system(', 
        b'<script'
    ]

    @staticmethod
    def validate_file(file_path):
        try:
            with open(file_path, 'rb') as f:
                content = f.read() # Read FULL content (warning: memory intensive for large files)
                
                # 1. Magic Number Check
                is_jpg = content.startswith(HardenedFileValidator.SIGNATURE_JPG)
                is_pdf = content.startswith(HardenedFileValidator.SIGNATURE_PDF)
                
                if not (is_jpg or is_pdf):
                    return "INVALID_HEADER"

                # 2. Deep Content Scan (The Fix)
                for bad_sig in HardenedFileValidator.BLACKLIST_SIGNATURES:
                    if bad_sig in content:
                        return "MALICIOUS_PAYLOAD_DETECTED"

                if is_jpg: return "VALID_JPG"
                if is_pdf: return "VALID_PDF"
                
                return "INVALID"
                
        except Exception as e:
            return "ERROR"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: validator_hardened.py <file>")
        sys.exit(1)
    print(HardenedFileValidator.validate_file(sys.argv[1]))
