#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP


#include <iostream>
#include <map>
#include <list>

class ServerConfig
{
 	public:

 	 	// A UTILISER

 	 	//donne le path complet de la resssource, apres alias ou root
 	 	//A VOIR: revoyer ou non une erreur si root depasse / fichier inexistant
		std::string	getFullPath(std::string uri, int location);

                //donne l'identifiant de la location, qui est a fournir dans tous les autres appels
                int getLocation(std::string uri);

                //Si une error page custom est fournie, renvoie sont PATH, sinon renvoie une empty string
                 std::string getCustomErrorPage(int loc, int errorCode);
 	 	//Renvoie le premier argument de la Directive
 	 	//Exemple: "autoindex yes;" -> getDirectiveParameter([uri], "autoindex") == "yes"
		std::string	getDirectiveParameter(int location, std::string directive_name);

 	 	//Renvoie les arguments de la Directive
		//Exemple: "error_page 404 /youdieded.html;" -> getDirectiveParameters([uri], "error_page") == {"404", "/youdieded.html"}
 	 	std::list<std::string>	getDirectiveParameters(int location, std::string directive_name);

 	 	//Verifie si l'agrument est present
 	 	//Exemple: "index GET POST DELETE;" -> inDirectiveParameters([uri], "index", "GET") == true
		bool		inDirectiveParameters(int location, std::string directive_name, std::string parameter);

		//Getter (address, port, server_name list)
		const std::string		&getAddress(void) const;
		const std::string		&getPort(void) const;
		const std::list<std::string>	&getServerNames(void) const;

		// Compare les infos requete avec le serveur
		bool		serverCmp(std::string address, std::string port, std::string server_name);

 	 	//La directive existe-elle dans le block ? A utiliser pour eviter les erreurs.
 	 	//Exemple: "patate 12 13;" -> inDirectives([uri], "patate") == true
	 	bool		inDirectives(int location, std::string directive_name);


};


#endif