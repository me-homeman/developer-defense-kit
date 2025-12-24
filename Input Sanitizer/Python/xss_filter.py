import re
from html.parser import HTMLParser
import html

class WhitelistFilter(HTMLParser):
    def __init__(self):
        super().__init__()
        self.reset()
        self.sanitized_data = []
        # Whitelist: Only these tags are allowed.
        self.allowed_tags = {'b', 'i', 'u', 'p', 'br', 'a', 'strong', 'em'}
        # Whitelist: Only these attributes are allowed on specific tags.
        self.allowed_attrs = {
            'a': {'href', 'title'},
            'img': {'src', 'alt'} # Note: img not in allowed_tags above, just example if added
        }

    def handle_starttag(self, tag, attrs):
        if tag in self.allowed_tags:
            clean_attrs = []
            for attr, value in attrs:
                # 1. Attribute Whitelisting
                if tag in self.allowed_attrs and attr in self.allowed_attrs[tag]:
                    # 2. Protocol Validation (No javascript:)
                    if attr in ['href', 'src']:
                        if value.strip().lower().startswith('javascript:'):
                            continue # Skip dangerous protocol
                    clean_attrs.append(f'{attr}="{html.escape(value)}"')
            
            # Reconstruct the tag
            if clean_attrs:
                self.sanitized_data.append(f'<{tag} {" ".join(clean_attrs)}>')
            else:
                self.sanitized_data.append(f'<{tag}>')

    def handle_endtag(self, tag):
        if tag in self.allowed_tags:
            self.sanitized_data.append(f'</{tag}>')

    def handle_data(self, data):
        # 3. Output Encoding for text content
        self.sanitized_data.append(html.escape(data))

    def get_clean_data(self):
        return "".join(self.sanitized_data)

def sanitize_input_v2(user_input):
    """
    V2.0: Uses a Whitelist approach via HTMLParser.
    Only allows specific safe tags. Encodes everything else.
    """
    parser = WhitelistFilter()
    parser.feed(user_input)
    return parser.get_clean_data()

def encode_output(user_input):
    """
    Strategy: Output Encoding.
    Converts special characters to HTML entities.
    Safest general approach if you don't need HTML formatting.
    """
    return html.escape(user_input)

def main():
    dirty_string = (
        "Hello. <script>alert('XSS')</script>\n"
        "<b>Bold Text is fine.</b>\n"
        "<a href='javascript:alert(1)'>Dangerous Link</a>\n"
        "<img src=x onerror=alert('Event Handler XSS')>\n"
        "<p onclick='stealCookies()'>Click me</p>"
    )

    print("Original Input:")
    print("-" * 40)
    print(dirty_string)
    print("-" * 40)

    # Approach 1: Whitelist Sanitization (Allow safe HTML, strip bad)
    clean_html = sanitize_input_v2(dirty_string)
    print("\n[Approach 1] Whitelist Sanitization (Allow safe HTML):")
    print("-" * 40)
    print(clean_html)
    print("-" * 40)

    # Approach 2: Output Encoding (Treat everything as text)
    encoded_text = encode_output(dirty_string)
    print("\n[Approach 2] Output Encoding (Safest for plain text):")
    print("-" * 40)
    print(encoded_text)
    print("-" * 40)

if __name__ == "__main__":
    main()
