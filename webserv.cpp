/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:26:45 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/02 11:58:45 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here  
   // terminate program  

   
}

int main() 
{
    Server server;
    int ret;
    signal(SIGINT, signalHandler);
    ret = server.start_server();
    return ret;
}
