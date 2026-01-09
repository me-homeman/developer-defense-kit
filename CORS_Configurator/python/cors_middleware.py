from flask import Flask, request, abort

app = Flask(__name__)

ALLOWED_ORIGIN = "https://yourfrontend.com"

@app.before_request
def check_cors():
    origin = request.headers.get('Origin')
    
    # If there is no Origin header, it's a same-origin request or non-browser request, usually allowed.
    # However, for strict CORS enforcement on cross-origin, we focus on presence of Origin.
    if origin:
        if origin == ALLOWED_ORIGIN:
            # We will attach headers in after_request
            pass
        else:
            # Block unauthorized origins
            abort(403, description="Forbidden: Invalid Origin")

@app.after_request
def inject_cors_headers(response):
    origin = request.headers.get('Origin')
    
    if origin == ALLOWED_ORIGIN:
        response.headers['Access-Control-Allow-Origin'] = ALLOWED_ORIGIN
        response.headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS'
        response.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'
        response.headers['Vary'] = 'Origin'
        
    return response

@app.route("/")
def index():
    return "Hello from the API!"

if __name__ == "__main__":
    app.run(port=5000)
