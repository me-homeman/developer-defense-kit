#include <iostream>
#include <string>
#include <regex>
#include <vector>

/**
 * V2.0: Enhanced Sanitization Strategy (Defense in Depth).
 * 
 * 1. Tag Stripping: Removes dangerous tags like <script>, <iframe>.
 * 2. Attribute Stripping: Removes dangerous attributes like 'onload', 'onclick'.
 * 3. Protocol Stripping: Removes 'javascript:' URIs.
 */
std::string sanitize_input_v2(std::string input) {
    // 1. Tag Stripping (Existing logic)
    std::regex tag_pattern(R"(<(script|iframe|object|embed)\b[^>]*>[\s\S]*?</\1>)", 
                           std::regex_constants::ECMAScript | std::regex_constants::icase);
    input = std::regex_replace(input, tag_pattern, "");

    // 2. Event Handler Stripping (e.g., onload=..., onerror=...)
    // Looks for ' on<event>=' pattern.
    std::regex event_pattern(R"(\s+on\w+\s*=)", 
                             std::regex_constants::ECMAScript | std::regex_constants::icase);
    input = std::regex_replace(input, event_pattern, " data-unsafe-attr=");

    // 3. Javascript Protocol Stripping (e.g., href="javascript:")
    std::regex js_proto_pattern(R"(javascript:)", 
                                std::regex_constants::ECMAScript | std::regex_constants::icase);
    input = std::regex_replace(input, js_proto_pattern, "unsafe:");

    return input;
}

/**
 * Output Encoding: The gold standard for XSS prevention.
 * Converts special characters into their HTML Entity equivalents.
 */
std::string html_encode(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

int main() {
    std::string dirty_string = 
        "Hello. <script>alert('XSS')</script>\n"
        "<b>Bold is safe.</b>\n"
        "<a href='javascript:alert(1)'>Dangerous Link</a>\n"
        "<img src=x onerror=alert('Event Handler XSS')>";

    std::cout << "Original Input:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << dirty_string << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // Approach 1: Enhanced Sanitization
    std::string clean_string = sanitize_input_v2(dirty_string);
    std::cout << "\n[Approach 1] Enhanced Sanitization (Filter Bad):" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << clean_string << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // Approach 2: Output Encoding
    std::string encoded_string = html_encode(dirty_string);
    std::cout << "\n[Approach 2] Output Encoding (Safest for Text):" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << encoded_string << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    return 0;
}
