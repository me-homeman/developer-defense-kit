#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <algorithm>

/**
 * Sanitizes the input string to prevent log injection.
 * Replaces newlines and other control characters with escape sequences.
 */
std::string sanitize(std::string input) {
    std::string output;
    output.reserve(input.length());
    for (char c : input) {
        if (c == '\n') {
            output += "\\n";
        } else if (c == '\r') {
            output += "\\r";
        } else if (c == '\t') {
            output += "\\t";
        } else if (c >= 0 && c < 32) {
            // Replace other non-printable characters with space
            output += ' '; 
        } else {
            output += c;
        }
    }
    return output;
}

/**
 * Logs suspicious activity to security.log.
 * 
 * Format: [TIMESTAMP] [SEVERITY_LEVEL] [IP_ADDRESS] [MESSAGE]
 */
void log_suspicious_activity(const std::string& severity, const std::string& ip_address, const std::string& message) {
    // Open file in append mode
    std::ofstream log_file("security.log", std::ios_base::app);
    
    if (log_file.is_open()) {
        // Get current time
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        
        // Sanitize inputs
        std::string safe_severity = sanitize(severity);
        std::string safe_ip = sanitize(ip_address);
        std::string safe_message = sanitize(message);

        // Write formatted log entry
        log_file << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
                 << "[" << safe_severity << "] "
                 << "[" << safe_ip << "] "
                 << safe_message << "\n";
        
        log_file.close();
    } else {
        std::cerr << "Error: Unable to open security.log for writing." << std::endl;
    }
}

int main() {
    std::cout << "Logging suspicious activities to security.log..." << std::endl;

    // Simulate failed login
    log_suspicious_activity("WARNING", "192.168.1.101", "Failed Login Attempt - User: root");

    // Simulate access denied
    log_suspicious_activity("ERROR", "203.0.113.88", "Access Denied - Invalid API Key");

    std::cout << "Done." << std::endl;
    return 0;
}