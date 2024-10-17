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
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <title>
            Login Page
        </title>
        <link rel="stylesheet" href="style.css"> 
    </head>
    <body>
        <div class="page">
            {content}
        </div>
    </body>
    </html>
    """)

def send_cookie(username):
    # Create a cookie
    cookie = http.cookies.SimpleCookie()
    cookie["session"] = username + ":" + SECRET_KEY
    # Set cookie expiration for 1 day
    expires = "Max-Age=30; SameSite=Strict"
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
    <div class="title">
            <h1>Login</h1>
        </div>
        <div class="bloc">
            <form method="POST" action="/session/session.py">
                <br>
                <label class="username-label">Username:</label><input type="text" name="username"><br>
                <label class="username-label">Password:</label><input type="password" name="password"><br>
                <br><input type="submit" value="Login" class="home-button">
            </form>
        </div>
        <a HREF="../index.html" class="home-button">go to home</a
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
        print_html(f"""
                    <div class="title">
                        <h2>Welcome, {username}!</h2>
                    </div>
                    <div class="bloc">
                        <p>You are now logged in.</p>
                    </div>
                    <div class="bloc">
                        <a HREF="../index.html">go to home</a>
                    </div>

                   """)
    else:
        # Invalid credentials
        print_header()
        print("\r\n", end='')
        print_html("<div class=\"title\"><h2>Invalid username or password</h2></div>" + login_form())

def main():

    # Check if the user is already logged in via cookie
    logged_in_user = check_cookie()
    if logged_in_user:
        print_header()
        print("\r\n", end='')
        print_html(f"""
                    <div class="title">
                        <h2>Welcome back, {logged_in_user}!</h2>
                    </div>
                    <div class="bloc">
                        <p><br>You are already logged in.</p>
                    </div>
                    <div class="bloc">
                        <a HREF="../index.html">go to home</a>
                    </div>
                    """)

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
