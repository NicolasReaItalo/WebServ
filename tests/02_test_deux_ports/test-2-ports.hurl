GET http://localhost:5000
Host: localhost
HTTP 200
[Asserts]
body contains "<h1>VIVE LES CHATS</h1>"

GET http://localhost:6000
Host: localhost
HTTP 200
[Asserts]
body contains "<h1>VIVE LES CHIENS</h1>"
