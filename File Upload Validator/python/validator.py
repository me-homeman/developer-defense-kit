import sys
import os

class FileValidator:
    SIGNATURE_JPG = b'\xFF\xD8\xFF'
    SIGNATURE_PDF = b'\x25\x50\x44\x46'

    @staticmethod
    def validate_file(file_path):
        try:
            with open(file_path, 'rb') as f:
                header = f.read(4)
                if header.startswith(FileValidator.SIGNATURE_JPG): return "VALID_JPG"
                if header.startswith(FileValidator.SIGNATURE_PDF): return "VALID_PDF"
                return "INVALID"
        except: return "ERROR"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: validator.py <file>")
        sys.exit(1)
    print(FileValidator.validate_file(sys.argv[1]))
