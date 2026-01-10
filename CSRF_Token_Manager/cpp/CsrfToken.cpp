#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>

class CsrfToken {
public:
    // Generates a 32-character hex token by reading from /dev/urandom
    static std::string generate_token() {
        std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
        if (!urandom) {
            throw std::runtime_error("Failed to open /dev/urandom");
        }

        unsigned char buffer[16];
        urandom.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
        if (!urandom) {
            throw std::runtime_error("Failed to read random bytes");
        }

        std::stringstream ss;
        for (int i = 0; i < 16; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
        }
        return ss.str();
    }

    // Validates the token using a strict constant-time comparison
    static bool validate_token(const std::string& received_token, const std::string& stored_token) {
        // Even if lengths differ, we process to avoid timing leaks regarding length.
        // However, we must ensure we don't access out of bounds.
        // To do this safely in constant time, we usually pick the length of one
        // and if they differ, the result is forced to fail, but we still loop.
        
        volatile size_t len_r = received_token.length();
        volatile size_t len_s = stored_token.length();
        
        // This 'diff' tracks if characters mismatch OR if lengths mismatch
        volatile unsigned char diff = (len_r != len_s);

        // We iterate up to the length of the stored token (assumed valid source of truth)
        // If received is shorter, we'll be comparing against garbage or need to handle it.
        // SAFE APPROACH for this snippet:
        // Since we are validating a fixed-size token (32 hex chars), we can enforce that first.
        
        if (len_r != 32 || len_s != 32) {
            // If the *expected* token isn't 32, something is wrong with our logic or storage.
            // If the *received* token isn't 32, it's definitely invalid.
            // To be strictly constant time, we would loop 32 times regardless.
            return false; // Acceptable optimization for fixed-length protocol violation.
        }

        for (size_t i = 0; i < 32; ++i) {
            diff |= (received_token[i] ^ stored_token[i]);
        }

        return diff == 0;
    }
};

int main() {
    try {
        std::string session_token = CsrfToken::generate_token();
        std::cout << "Generated Token: " << session_token << std::endl;

        std::string incoming_token = session_token;
        
        bool is_valid = CsrfToken::validate_token(incoming_token, session_token);
        std::cout << "Token Valid? " << (is_valid ? "Yes" : "No") << std::endl;

        std::string attack_token = "deadbeefdeadbeefdeadbeefdeadbeef";
        bool is_attack_valid = CsrfToken::validate_token(attack_token, session_token);
        std::cout << "Attack Token Valid? " << (is_attack_valid ? "Yes" : "No") << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}