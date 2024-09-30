/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_cgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 17:35:59 by nrea              #+#    #+#             */
/*   Updated: 2024/09/30 18:58:36 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

header_infos handle_cgi(header_infos & response, std::string cgi, ConfigServer *config,
int locationIndex, std::map<std::string,std::string> &header_attributes)
{
	//On verifie que ce cgi est autorise
	std::string interpreter = config->getDirectiveOutput(locationIndex, "cgi", cgi);
	if (interpreter.empty())
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	// ON verifie que le path vers l'interpreter existe et est executable ce serait bete
	FileInfos interpreter_path(interpreter);
	if (!interpreter_path.file_exist() || interpreter_path.exec_permission())
		return response_error(HTTP_STATUS_INTERNAL_SERVER_ERROR, config, locationIndex);
	// strip d'un eventuel PATH_INFO
	stripPathInfo(header_attributes,cgi);
	// Le script existe et est autorise en lecture ?
	FileInfos script_path(header_attributes["URI"]);
	if (!script_path.file_exist())
		return response_error(HTTP_STATUS_NOT_FOUND, config, locationIndex);
	if (!script_path.read_permission())
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);

	// On prepare notre environement;
	std::map<std::string,std::string> envMap;

	envMap["SERVER_SOFTWARE"]= "webserv";
	envMap["SERVER_NAME"]= "webserv";
	envMap["REQUEST_METHOD"]= "GET";
	envMap["PATH_INFO"]= header_attributes["PATH_INFO"];
	envMap["SCRIPT_NAME"]= "cgi-get.py";
	envMap["QUERY_STRING"]= "first_name=Nicolas&last_name=Rea";
	envMap["CONTENT_LENGTH"]= "0";
	envMap["HTTP_ACCEPT"]= "*/*";
	envMap["HTTP_ACCEPT_LANGUAGE"]= "*/*";
	envMap["HTTP_USER_AGENT"]= "Mozilla";
	envMap["HTTP_COOKIE"]= "";


	// et notre reponse
	response.toDo = GET_CGI;
	response.interpreterPath = interpreter;
	response.ressourcePath = header_attributes["URI"];



	return response;
}


/*
Liste des variables CGI
Variables en rapport avec le serveur
SERVER_SOFTWARE
Le nom et la version du serveur HTTP répondant à la requête. (Format : nom/version)
SERVER_NAME
Le nom d'hôte, alias DNS ou adresse IP du serveur.
GATEWAY_INTERFACE
La révision de la spécification CGI que le serveur utilise. (Format : CGI/révision)
Variables spécifiques à la requête
SERVER_PROTOCOL
Le nom et la révision du protocole dans lequel la requête a été faite (Format : protocole/révision)
SERVER_PORT
Le numéro de port sur lequel la requête a été envoyée.
REQUEST_METHOD
La méthode utilisée pour faire la requête. Pour HTTP, elle contient généralement « GET » ou « POST ».
PATH_INFO
Le chemin supplémentaire du script tel que donné par le client. Par exemple, si le serveur héberge le script « /cgi-bin/monscript.cgi » et que le client demande l'url « http://serveur.org/cgi-bin/monscript.cgi/marecherche », alors PATH_INFO contiendra « marecherche ».
PATH_TRANSLATED
Contient le chemin demandé par le client après que les conversions virtuel → physique ont été faites par le serveur.
SCRIPT_NAME
Le chemin virtuel vers le script étant exécuté. Exemple : « /cgi-bin/script.cgi »
QUERY_STRING
Contient tout ce qui suit le « ? » dans l'URL envoyée par le client. Toutes les variables provenant d'un formulaire envoyé avec la méthode « GET » seront contenues dans le QUERY_STRING sous la forme « var1=val1&var2=val2&... ».
REMOTE_HOST
Le nom d'hôte du client. Si le serveur ne possède pas cette information (par exemple, lorsque la résolution DNS inverse est désactivée), REMOTE_HOST sera vide.
REMOTE_ADDR
L'adresse IP du client.
AUTH_TYPE
Le type d'identification utilisé pour protéger le script (s’il est protégé et si le serveur supporte l'identification).
REMOTE_USER
Le nom d'utilisateur du client, si le script est protégé et si le serveur supporte l'identification.
REMOTE_IDENT
Nom d'utilisateur (distant) du client faisant la requête. Le serveur doit supporter l'identification RFC 931. Cette variable devrait être utilisée à des fins de journaux seulement.
CONTENT_TYPE
Le type de contenu attaché à la requête, si des données sont attachées (comme lorsqu'un formulaire est envoyé avec la méthode « POST »).
CONTENT_LENGTH
La longueur du contenu envoyé par le client.
Variables provenant du client
Toutes les variables qui sont envoyées par le client sont aussi passées au script CGI, après que le serveur a rajouté le préfixe « HTTP_ ». Voici quelques exemples de variables possibles :

HTTP_ACCEPT
Les types de données MIME que le client accepte de recevoir.
Exemple : text/*, image/jpeg, image/png, image/
HTTP_ACCEPT_LANGUAGE
Les langues dans lequel le client accepte de recevoir la réponse.
Exemple : fr_CA, fr
HTTP_USER_AGENT
Le navigateur utilisé par le client.
Exemple : Mozilla/5.0 (compatible; Konqueror/3; Linux)
HTTP_COOKIE
Les éventuels cookies. Une liste de paires clef=valeur contenant les cookies positionnés par le site, séparés par des points-virgules.
HTTP_REFERER
Une adresse absolue ou partielle de la page web à partir de laquelle la requête vers la page courante a été émise.

*/
