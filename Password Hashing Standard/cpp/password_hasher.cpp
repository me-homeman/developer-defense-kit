#include <sodium.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>
#include <memory>

/**
 * Secure Allocator using libsodium's guarded memory.
 * - Prevents swapping to disk (if possible/configured).
 * - Can detect buffer overflows (canary).
 * - Automatically zeroes memory on deallocation.
 */
template <typename T>
struct SodiumAllocator {
    using value_type = T;

    SodiumAllocator() = default;

    template <class U>
    constexpr SodiumAllocator(const SodiumAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }
        // sodium_alloc returns a pointer to a region protected by a guard page
        // and optionally locked in memory (mlock).
        void* p = sodium_alloc(n * sizeof(T));
        if (!p) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) noexcept {
        sodium_free(p); // Securely frees and wipes memory
    }
};

template <typename T, typename U>
bool operator==(const SodiumAllocator<T>&, const SodiumAllocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const SodiumAllocator<T>&, const SodiumAllocator<U>&) { return false; }

// Define a SecureString type that uses the secure allocator
using SecureString = std::basic_string<char, std::char_traits<char>, SodiumAllocator<char>>;

class PasswordUtility {
public:
    PasswordUtility() {
        if (sodium_init() < 0) {
            throw std::runtime_error("libsodium failed to initialize");
        }
    }

    /**
     * Hashes a password. 
     * Accepts SecureString to ensure input hygiene.
     */
    std::string hash_password(const SecureString& password) {
        char hash[crypto_pwhash_STRBYTES];

        if (crypto_pwhash_str(
                hash,
                password.c_str(),
                password.length(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            throw std::runtime_error("Out of memory or other error during hashing");
        }

        return std::string(hash);
    }

    /**
     * Verifies a password.
     * returns: true if valid, false otherwise
     */
    bool check_password(const std::string& hash_str, const SecureString& password) {
        if (crypto_pwhash_str_verify(
                hash_str.c_str(),
                password.c_str(),
                password.length()) == 0) {
            
            // Check if hash needs upgrade (rehash) 
            // crypto_pwhash_str_needs_rehash is available in newer libsodium versions
            // Here we verify simply; full rehash logic mirrors the Python example conceptually
            return true;
        }
        return false;
    }
    
    /**
     * Check if the hash parameters match our current policy.
     * This allows upgrading users from "Interactive" to "Sensitive" (harder) difficulty over time.
     */
    bool check_needs_rehash(const std::string& hash_str) {
         int result = crypto_pwhash_str_needs_rehash(
            hash_str.c_str(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE, 
            crypto_pwhash_MEMLIMIT_INTERACTIVE
        );
        // result is 1 if rehash needed, 0 if not, -1 on error
        return result == 1;
    }
};

int main() {
    std::cout << "--- C++ Argon2id Password Utility (Secure Memory) ---" << std::endl;

    try {
        PasswordUtility tool;

        // 1. Use SecureString for the sensitive password
        // This memory is allocated via sodium_alloc
        SecureString password = "MySecurePassword123!";
        
        std::cout << "\n[+] Hashing password..." << std::endl;
        std::string hashed = tool.hash_password(password);
        std::cout << "[+] Generated Hash: " << hashed << std::endl;

        // 2. Verifying
        std::cout << "\n[+] Verifying correct password..." << std::endl;
        bool is_valid = tool.check_password(hashed, password);
        std::cout << "Result: " << (is_valid ? "Success" : "Failure") << std::endl;
        
        // 3. Check for rehash (won't be needed here as we just made it)
        if (tool.check_needs_rehash(hashed)) {
            std::cout << "[!] Hash needs upgrade (rehash)." << std::endl;
        } else {
            std::cout << "[+] Hash is up to date." << std::endl;
        }

        // When 'password' goes out of scope here, SodiumAllocator::deallocate is called,
        // invoking sodium_free() which wipes the memory.

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}