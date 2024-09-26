# requete GET  simple
GET http://localhost:8080/
User-Agent: Mozilla/5.0
Accept: */*
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
HTTP 200


# requete GET sur fichier non existant
GET http://localhost:8080/not-exist
User-Agent: Mozilla/5.0
Accept: */*
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
HTTP 404

# requete GET sur fichier interdit
GET http://localhost:8080/forbidden.html
User-Agent: Mozilla/5.0
Accept: */*
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
HTTP 403


# requete GET sur fichier interdit
GET http://localhost:8080/forbidden.html
User-Agent: Mozilla/5.0
Accept: */*
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
HTTP 403


# requete GET sur repertoire avec directive index
GET http://localhost:8080/dir
User-Agent: Mozilla/5.0
Accept: */*
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
HTTP 200


POST http://localhost:8080/correction.pdf
Content-Type: application/pdf
file,webserv-correction.pdf;
