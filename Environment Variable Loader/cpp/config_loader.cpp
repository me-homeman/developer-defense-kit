#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include <vector>
#include <algorithm>

class ConfigLoader {
private:
    std::map<std::string, std::string> env_map;

    // Trim whitespace from string
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (std::string::npos == first) return str;
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    void load_dotenv() {
        std::ifstream file(".env");
        if (!file.is_open()) {
            // It's okay if .env is missing, we might rely purely on system env vars
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Simple parsing: KEY=VALUE
            // Ignore comments
            if (line.empty() || line[0] == '#') continue;

            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = trim(line.substr(0, delimiterPos));
                std::string value = trim(line.substr(delimiterPos + 1));
                env_map[key] = value;
            }
        }
        file.close();
    }

public:
    ConfigLoader() {
        load_dotenv();
    }

    std::string get_secret(const std::string& key) {
        // 1. Check System Environment Variable (Priority)
        const char* system_val = std::getenv(key.c_str());
        std::string value;

        if (system_val != nullptr) {
            value = std::string(system_val);
        } else {
            // 2. Check loaded .env values
            if (env_map.find(key) != env_map.end()) {
                value = env_map[key];
            } else {
                // 3. Fail Fast
                std::cerr << "CRITICAL ERROR: Missing required environment variable: '" << key << "'" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }

        // 4. Safety Check (Hardcode Prevention)
        std::vector<std::string> unsafe_placeholders = {"REPLACE_ME", "YOUR_PASSWORD_HERE", "CHANGE_THIS"};
        for (const auto& placeholder : unsafe_placeholders) {
            if (value == placeholder) {
                std::cerr << "SECURITY ALERT: Default placeholder detected for '" << key << "'. Update your configuration." << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }

        return value;
    }
};

std::string mask_secret(const std::string& secret) {
    if (secret.length() <= 4) return "****";
    return secret.substr(0, 2) + "****" + secret.substr(secret.length() - 2);
}

int main() {
    ConfigLoader loader;
    std::string target_key = "API_KEY";

    std::cout << "Attempting to load " << target_key << "..." << std::endl;
    std::string secret = loader.get_secret(target_key);

    std::cout << "Success! Securely loaded " << target_key << ": " << mask_secret(secret) << std::endl;

    return 0;
}
