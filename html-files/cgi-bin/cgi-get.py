#!/usr/bin/python

import cgi, cgitb

form = cgi.FieldStorage()

first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')

header=""
body=""
body += "<html><head><title>Hello - GET CGI Program</title></head>"
body += f"""<body>
<div class="page">
<div class="title">
<h1>This an exemple of a GET CGI</h1>
</div>
<div class="bloc">
<h2>Hello {first_name} {last_name}</h2>
</div></div>
</body>
</html>
"""

# header += "HTTP/1.1 200 OK\r\n"
header += "Content-type: text/html\r\n"
# header += f"Content-Length: {len(body.encode('utf-8'))}\r\n"

print(f"{header}\r\n{body}",end='')
