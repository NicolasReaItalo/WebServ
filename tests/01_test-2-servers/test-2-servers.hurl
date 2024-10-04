GET http://localhost:8080
Host: vive-les-chats.fr
HTTP 200
[Asserts]
body contains "<h1>VIVE LES CHATS</h1>"

GET http://localhost:8080
Host: vive-les-chiens.fr
HTTP 200
[Asserts]
body contains "<h1>VIVE LES CHIENS</h1>"
