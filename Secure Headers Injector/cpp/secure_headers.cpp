#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Simulating a Header map common in C++ web frameworks (like Crow, Drogon, or Oat++)
using HeaderMap = std::unordered_map<std::string, std::string>;

class SecureHeadersInjector {
public:
    /**
     * Injects essential security headers into the provided header map.
     * 
     * @param headers Reference to the response headers map.
     */
    static void inject(HeaderMap& headers) {
        // List of headers we intend to set
        const std::vector<std::string> keys_to_set = {
            "Content-Security-Policy",
            "X-Frame-Options",
            "Strict-Transport-Security",
            "X-Content-Type-Options"
        };

        // Remove existing headers with case-insensitive matching
        // Note: Modifying a container while iterating requires care.
        // We collect keys to remove first.
        std::vector<std::string> keys_to_remove;
        for (const auto& pair : headers) {
            for (const auto& secure_key : keys_to_set) {
                if (caseInsensitiveEquals(pair.first, secure_key)) {
                    keys_to_remove.push_back(pair.first);
                }
            }
        }

        for (const auto& key : keys_to_remove) {
            headers.erase(key);
        }

        // CSP: Restrict resources to same origin by default
        headers["Content-Security-Policy"] = "default-src 'self'";
        
        // X-Frame-Options: Prevent Clickjacking
        headers["X-Frame-Options"] = "DENY";
        
        // HSTS: Enforce HTTPS for 1 year
        headers["Strict-Transport-Security"] = "max-age=31536000; includeSubDomains";
        
        // X-Content-Type-Options: Prevent MIME-sniffing
        headers["X-Content-Type-Options"] = "nosniff";
    }

private:
    static bool caseInsensitiveEquals(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) return false;
        return std::equal(a.begin(), a.end(), b.begin(),
                          [](char a, char b) {
                              return tolower(a) == tolower(b);
                          });
    }
};

// --- Mock Usage / Test Case ---
#ifdef BUILD_MAIN
int main() {
    std::cout << "--- C++ Secure Headers Injector Demo ---" << std::endl;

    // Simulate existing headers from a web server response
    HeaderMap response_headers;
    response_headers["Content-Type"] = "application/json";
    response_headers["Connection"] = "keep-alive";

    std::cout << "Original Headers:" << std::endl;
    for (const auto& pair : response_headers) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }

    std::cout << "\n[+] Injecting security headers..." << std::endl;
    SecureHeadersInjector::inject(response_headers);

    std::cout << "\nFinal Headers:" << std::endl;
    for (const auto& pair : response_headers) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}
#endif
