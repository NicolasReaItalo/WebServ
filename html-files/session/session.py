#!/usr/bin/env python3

import cgi
import http.cookies
import os
import datetime

# A simple in-memory user database
USER_DB = {
    "admin": "password123",
    "user1": "mypassword"
}

# Define a secret key to represent logged-in users (for simplicity)
SECRET_KEY = "supersecretkey"

def print_header():
    print("Content-Type: text/html\r\n", end='')

def print_html(content):
    print(f"""
    <html>
    <head><title>Login Page</title></head>
    <body>
        {content}
    </body>
    </html>
    """)

def send_cookie(username):
    # Create a cookie
    cookie = http.cookies.SimpleCookie()
    cookie["session"] = username + ":" + SECRET_KEY
    # Set cookie expiration for 1 day
    expires = "Max-Age=360; SameSite=Strict"
    cookie["session"]["Max-Age"] = expires
    print(cookie.output(header="Set-Cookie:") + "\r\n", end='')

def check_cookie():
    if "HTTP_COOKIE" in os.environ:
        cookie = http.cookies.SimpleCookie(os.environ["HTTP_COOKIE"])
        if "session" in cookie:
            session_data = cookie["session"].value.split(":")
            if len(session_data) == 2:
                username, key = session_data
                # Validate cookie
                if key == SECRET_KEY and username in USER_DB:
                    return username
    return None

def login_form():
    return """
    <h1>Login</h1>
    <form method="POST" action="/session/session.py">
        Username: <input type="text" name="username"><br>
        Password: <input type="password" name="password"><br>
        <input type="submit" value="Login">
    </form>
    """

def process_login():
    # Get form data
    form = cgi.FieldStorage()
    username = form.getvalue("username")
    password = form.getvalue("password")
    
    # Check credentials
    if username in USER_DB and USER_DB[username] == password:
        # Valid credentials, set a cookie
        print_header()
        send_cookie(username)
        print("\r\n", end='')
        print_html(f"<h2>Welcome, {username}!</h2><p>You are now logged in.</p>")
    else:
        # Invalid credentials
        print_header()
        print("\r\n", end='')
        print_html("<h2>Invalid username or password</h2>" + login_form())

def main():

    # Check if the user is already logged in via cookie
    logged_in_user = check_cookie()
    if logged_in_user:
        print_header()
        print("\r\n", end='')
        print_html(f"<h2>Welcome back, {logged_in_user}!</h2><p>You are already logged in.</p>")

    else:
        # If not logged in, either show the login form or process the login
        if os.environ['REQUEST_METHOD'] == 'POST':
            process_login()
        else:
            print_header()
            print("\r\n", end='')
            print_html(login_form())

if __name__ == "__main__":
    main()
