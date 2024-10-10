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

#Requete GET vers dossier autoindex on sans directive index
GET http://localhost:8080/autoindex_ok
HTTP 200

#Requete GET vers dossier autoindex off sans directive index
GET http://localhost:8080/autoindex_off
HTTP 403

#Requete GET vers uri n'existant pas avec un message d'erreur custom
GET http://localhost:8080/misc/not_found
HTTP 404
[Asserts]
body contains "<h1>CUSTOM ERROR 404</h1>"

#Requete GET vers uri interdite avec un message d'erreur custom
GET http://localhost:8080/misc/
HTTP 404
[Asserts]
body contains "<h1>CUSTOM ERROR 403</h1>"
