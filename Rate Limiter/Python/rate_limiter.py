import time
import threading
from collections import defaultdict

class RateLimiter:
    """
    A thread-safe sliding-window rate limiter.

    This implementation tracks request timestamps for each IP address.
    """
    def __init__(self, max_requests, window_seconds):
        """
        Initializes the RateLimiter.

        :param max_requests: Maximum number of requests allowed in a window.
        :param window_seconds: The time window in seconds.
        """
        self.max_requests = max_requests
        self.window_seconds = window_seconds
        # A dictionary to store lists of request timestamps for each IP
        self.requests = defaultdict(list)
        # A lock to make the class thread-safe
        self.lock = threading.Lock()
        # Last cleanup time
        self.last_cleanup_time = time.time()
        self.cleanup_interval = window_seconds * 2 # Cleanup every two windows

    def _cleanup_old_entries(self):
        """
        Removes old, expired IP entries to prevent memory from growing indefinitely.
        An entry is considered old if the latest request was more than a window ago.
        """
        current_time = time.time()
        with self.lock:
             # Check if it's time to clean up
            if current_time - self.last_cleanup_time < self.cleanup_interval:
                return

            ips_to_delete = []
            for ip, timestamps in self.requests.items():
                if not timestamps or current_time - timestamps[-1] > self.window_seconds:
                    ips_to_delete.append(ip)

            for ip in ips_to_delete:
                del self.requests[ip]
            
            self.last_cleanup_time = current_time


    def check_rate_limit(self, ip_address):
        """
        Checks if an IP address has exceeded the rate limit using a sliding window log.

        :param ip_address: The IP address to check.
        :return: True if the request is allowed, False otherwise.
        """
        self._cleanup_old_entries() # Periodically clean up old entries

        current_time = time.time()
        with self.lock:
            # Get the list of timestamps for the current IP
            timestamps = self.requests[ip_address]

            # Remove timestamps that are outside the current window
            while timestamps and timestamps[0] <= current_time - self.window_seconds:
                timestamps.pop(0)

            # If the number of requests is still within the limit, allow and record it
            if len(timestamps) < self.max_requests:
                timestamps.append(current_time)
                return True  # Allowed
            else:
                return False  # Blocked

# Example Usage
if __name__ == '__main__':
    # Stricter limits for demonstration
    limiter = RateLimiter(max_requests=5, window_seconds=10)

    def make_request(ip):
        if limiter.check_rate_limit(ip):
            print(f"Request from {ip} ALLOWED.")
        else:
            print(f"Request from {ip} BLOCKED.")

    # Simulate rapid requests from one IP
    print("--- Simulating rapid requests from 192.168.1.1 ---")
    for _ in range(7):
        make_request("192.168.1.1")
        time.sleep(0.5)

    # Wait for part of the window to slide
    print("\n--- Waiting for 5 seconds... ---")
    time.sleep(5)

    # The window has partially slid, so some new requests should be allowed
    print("\n--- Making more requests, some should now be allowed ---")
    for _ in range(5):
        make_request("192.168.1.1")
        time.sleep(1)
        
    print(f"\n--- Checking memory, number of IPs tracked: {len(limiter.requests)} ---")
    print("--- Waiting for cleanup to run... ---")
    time.sleep(25)
    limiter._cleanup_old_entries()
    print(f"--- After cleanup, number of IPs tracked: {len(limiter.requests)} ---")