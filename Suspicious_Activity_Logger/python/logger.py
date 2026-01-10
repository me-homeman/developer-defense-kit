import logging
import sys

class SecurityLogger:
    """
    A custom logger to track suspicious activities like Failed Login Attempts
    and Access Denied errors.
    """
    def __init__(self, log_file='security.log'):
        self.logger = logging.getLogger('SecurityLogger')
        self.logger.setLevel(logging.INFO)
        
        # Avoid adding multiple handlers if the logger already exists
        if not self.logger.handlers:
            # Create file handler in append mode
            fh = logging.FileHandler(log_file, mode='a')
            
            # Create formatter matching requirements: [TIMESTAMP] [SEVERITY_LEVEL] [IP_ADDRESS] [MESSAGE]
            # We use a custom format string. 'ip' will be passed via the 'extra' dictionary.
            formatter = logging.Formatter('[%(asctime)s] [%(levelname)s] [%(ip)s] %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
            fh.setFormatter(formatter)
            
            self.logger.addHandler(fh)

    def log(self, severity, ip_address, message):
        """
        Logs a message with the given severity and IP address.
        Sanitizes the message to prevent log injection.
        """
        # Sanitize input: replace newlines with escaped characters
        safe_message = message.replace('\n', '\\n').replace('\r', '\\r')
        
        extra = {'ip': ip_address}
        lvl = severity.upper()
        
        if lvl == 'DEBUG':
            self.logger.debug(safe_message, extra=extra)
        elif lvl == 'INFO':
            self.logger.info(safe_message, extra=extra)
        elif lvl == 'WARNING':
            self.logger.warning(safe_message, extra=extra)
        elif lvl == 'ERROR':
            self.logger.error(safe_message, extra=extra)
        elif lvl == 'CRITICAL':
            self.logger.critical(safe_message, extra=extra)
        else:
            self.logger.info(safe_message, extra=extra)

if __name__ == "__main__":
    # Demonstration of the logger
    sec_log = SecurityLogger()
    
    print("Logging suspicious activities to security.log...")
    
    # Simulate failed login
    sec_log.log("WARNING", "192.168.1.101", "Failed Login Attempt - User: admin")
    
    # Simulate access denied
    sec_log.log("ERROR", "203.0.113.42", "Access Denied - Unauthorized directory access")
    
    print("Done.")
