#Requete GET autorisee
GET http://localhost:8080/index.html
Host: localhost
HTTP 200
[Asserts]

#Requete GET autorisee si index set
GET http://localhost:8080/index_set
HTTP 200

#Requete GET interdite si index  non set
GET http://localhost:8080/index_not_set
HTTP 403


#Requete GET vers fichier inexistant
GET http://localhost:8080/not-existing
HTTP 404

#Requete GET vers fichier interdit en lecture
GET http://localhost:8080/file_forbidden.html
HTTP 403

