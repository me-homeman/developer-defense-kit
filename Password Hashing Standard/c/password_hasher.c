#include <sodium.h>
#include <stdio.h>
#include <string.h>

int main() {
    // Initialize libsodium
    if (sodium_init() < 0) {
        fprintf(stderr, "libsodium failed to initialize\n");
        return 1;
    }

    printf("--- C Argon2id Password Utility (Locked Memory) ---
");

    // SECURE BUFFER:
    // We allocate a buffer and "lock" it in RAM. 
    // This prevents the OS from writing this memory to the hard drive (swap/pagefile).
    char password[128]; 

    // Lock the memory before putting sensitive data in it
    if (sodium_mlock(password, sizeof(password)) != 0) {
        fprintf(stderr, "Failed to lock memory! (Is strict sandbox enabled?)\n");
        return 1;
    }

    // Copy sensitive data into locked memory
    // In a real app, read directly into this buffer using fgets
    snprintf(password, sizeof(password), "MySecurePassword123!");
    size_t password_len = strlen(password);

    printf("\n[+] Hashing password (from locked memory)...
");

    // Buffer for the hash
    char hash[crypto_pwhash_STRBYTES];

    // 1. Hash the password
    if (crypto_pwhash_str(hash, password, password_len, 
                          crypto_pwhash_OPSLIMIT_INTERACTIVE, 
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Out of memory or other error during hashing\n");
        // Ensure we unlock before exit
        sodium_munlock(password, sizeof(password));
        return 1;
    }

    printf("[+] Generated Hash: %s\n", hash);

    // 2. Verify
    printf("\n[+] Verifying password...
");
    if (crypto_pwhash_str_verify(hash, password, password_len) == 0) {
        printf("Result: Success\n");
        
        // Check if rehash is needed (e.g., if we changed OPSLIMIT policy)
        int rehash_needed = crypto_pwhash_str_needs_rehash(hash, 
                                            crypto_pwhash_OPSLIMIT_INTERACTIVE, 
                                            crypto_pwhash_MEMLIMIT_INTERACTIVE);
        if (rehash_needed == 1) {
             printf("[!] Note: This hash should be upgraded (rehashed) to match new security params.\n");
        }
    } else {
        printf("Result: Failure\n");
    }

    // MEMORY SAFETY: 
    // sodium_munlock() automatically fills the memory with zeros before unlocking.
    // This is safer than manually calling memset/sodium_memzero.
    printf("\n[+] Unlocking and wiping password memory...
");
    sodium_munlock(password, sizeof(password));
    
    return 0;
}