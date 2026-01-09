#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Mock Request class
struct Request {
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
};

// Mock Response class
struct Response {
    int status_code;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class CORSFilter {
private:
    const std::string ALLOWED_ORIGIN = "https://yourfrontend.com";

public:
    // Process the request and modify the response headers accordingly
    bool process(const Request& req, Response& res) {
        std::string origin = "";
        bool origin_found = false;

        // Case-insensitive lookup for "Origin" header
        for (const auto& kv : req.headers) {
            std::string key = kv.first;
            // Simple tolower for case-insensitive check
            for (auto &c : key) c = tolower(c);
            
            if (key == "origin") {
                origin = kv.second;
                origin_found = true;
                break;
            }
        }
        
        // If Origin header is not present, it might be same-origin or non-browser. 
        // We typically allow processing to continue, but don't add CORS headers.
        if (!origin_found) {
            return true;
        }

        if (origin == ALLOWED_ORIGIN) {
            res.headers["Access-Control-Allow-Origin"] = ALLOWED_ORIGIN;
            res.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
            res.headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";
            res.headers["Vary"] = "Origin";
            return true; // Allowed
        } else {
            // Block unauthorized origins
            res.status_code = 403;
            res.body = "Forbidden: Invalid Origin";
            return false; // Request blocked
        }
    }
};

int main() {
    CORSFilter cors;

    // Test Case 1: Valid Origin
    Request req1;
    req1.headers["Origin"] = "https://yourfrontend.com";
    Response res1;
    res1.status_code = 200; // Assume success initially

    if (cors.process(req1, res1)) {
        std::cout << "Test 1 (Valid Origin): Allowed. Headers:" << std::endl;
        for (const auto& kv : res1.headers) {
            std::cout << kv.first << ": " << kv.second << std::endl;
        }
    } else {
        std::cout << "Test 1 (Valid Origin): Blocked." << std::endl;
    }

    std::cout << "--------------------------------" << std::endl;

    // Test Case 2: Invalid Origin
    Request req2;
    req2.headers["Origin"] = "https://malicious.com";
    Response res2;
    res2.status_code = 200;

    if (cors.process(req2, res2)) {
        std::cout << "Test 2 (Invalid Origin): Allowed." << std::endl;
    } else {
        std::cout << "Test 2 (Invalid Origin): Blocked with status " << res2.status_code << std::endl;
    }

    return 0;
}
