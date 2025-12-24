#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/**
 * Helper function: Case-insensitive string search.
 */
char* strcasestr_custom(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle)) {
            const char *h = haystack, *n = needle;
            while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
                h++; n++;
            }
            if (!*n) return (char*)haystack;
        }
    }
    return NULL;
}

/**
 * V2.0 Helper: Replace substring with spaces (sanitization).
 * Preserves length but neutralizes the threat.
 */
void mask_substring(char *start, size_t length) {
    for(size_t i = 0; i < length; i++) {
        start[i] = ' '; // Replace with space
    }
}

/**
 * V2.0: Enhanced Sanitization Strategy (Defense in Depth).
 * Modifies string in-place.
 */
void sanitize_input_v2(char *input) {
    // 1. Tag Stripping (Simplified for C demo: remove <script...>)
    // ... (Your previous tag removal logic logic works, skipping for brevity of this upgrade demo 
    // to focus on the new features. In a real full file, you'd keep the tag logic too).
    // Let's re-implement a simple tag stripper for <script> and <iframe>
    
    const char *tags[] = {"script", "iframe"};
    for (int t = 0; t < 2; t++) {
        char open_tag[20]; sprintf(open_tag, "<%s", tags[t]);
        char *found = strcasestr_custom(input, open_tag);
        while (found) {
            // Find closing tag </tag>
            char close_tag[20]; sprintf(close_tag, "</%s>", tags[t]);
            char *end = strcasestr_custom(found, close_tag);
            if (end) {
                size_t len = (end + strlen(close_tag)) - found;
                mask_substring(found, len); // Neutralize by blanking out
            } else {
                // Malformed/open-ended, just blank out the opening tag
                mask_substring(found, strlen(open_tag));
            }
            found = strcasestr_custom(input, open_tag);
        }
    }

    // 2. Event Handler Stripping (e.g. onmouseover=)
    // Find " on" followed by characters and "="
    // This is hard in pure C without regex. We will scan for " on"
    char *p = input;
    while ((p = strcasestr_custom(p, " on"))) {
        // p points to " on..."
        // Check if next char is alpha
        if (isalpha(p[3])) {
            // Check if it eventually has an '=' before a '>' or space
            char *eq = strchr(p, '=');
            char *space = strchr(p + 3, ' ');
            char *tag_end = strchr(p, '>');
            
            // Heuristic: if '=' comes before '>' it's likely an attribute
            if (eq && tag_end && eq < tag_end) {
                 // Mask the "on" part to "no"
                 p[1] = 'n'; p[2] = 'o'; // " no..." neutralizes the handler
            }
        }
        p++; 
    }

    // 3. Protocol Stripping (javascript:)
    char *js = strcasestr_custom(input, "javascript:");
    while (js) {
        mask_substring(js, 10); // Mask "javascript" -> "          "
        js = strcasestr_custom(input, "javascript:");
    }
}

/**
 * Output Encoding: Converts special chars to entities.
 * Returns a new allocated string (caller must free).
 */
char* html_encode(const char *input) {
    size_t len = strlen(input);
    size_t new_len = len * 6 + 1; // Worst case: every char is &quot; (6 chars)
    char *output = (char*)malloc(new_len);
    char *p = output;

    for (size_t i = 0; i < len; i++) {
        switch(input[i]) {
            case '&': strcpy(p, "&amp;"); p += 5; break;
            case '<': strcpy(p, "&lt;"); p += 4; break;
            case '>': strcpy(p, "&gt;"); p += 4; break;
            case '"': strcpy(p, "&quot;"); p += 6; break;
            case '\'': strcpy(p, "&apos;"); p += 6; break;
            default: *p++ = input[i]; break;
        }
    }
    *p = '\0';
    return output;
}

int main() {
    // Using a writable buffer
    char buffer[1024];
    const char *source = 
        "Hello. <script>alert('XSS')</script>\n"
        "<b>Bold is safe.</b>\n"
        "<a href='javascript:alert(1)'>Dangerous Link</a>\n"
        "<img src=x onerror=alert('Event Handler XSS')>";
    
    strcpy(buffer, source);

    printf("Original Input:\n--------------------\n%s\n--------------------\n", buffer);

    // Approach 1: Sanitization
    sanitize_input_v2(buffer);
    printf("\n[Approach 1] Enhanced Sanitization:\n--------------------\n%s\n--------------------\n", buffer);

    // Reset buffer for encoding demo
    strcpy(buffer, source);
    
    // Approach 2: Output Encoding
    char *encoded = html_encode(buffer);
    printf("\n[Approach 2] Output Encoding:\n--------------------\n%s\n--------------------\n", encoded);
    
    free(encoded);
    return 0;
}
