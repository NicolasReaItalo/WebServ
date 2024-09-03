/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:26:45 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/03 17:40:20 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"
#include <pthread.h>
#include <signal.h>


volatile sig_atomic_t stopper = 0;

void handle_sigint(int sig) {
    std::cout << "Caught signal " << sig << ", stopping server..." << std::endl;
    stopper = 1;
}

void* runServer(void* arg) {
    Server* server = static_cast<Server*>(arg);
    int ret = server->start_server();
    
    if (ret != 0) {
        std::cerr << "Server failed to start." << std::endl;
    }
    return NULL;
}

int main() 
{
    Server server1(8080, "127.0.0.1");
    Server server2(8081, "127.0.0.1"); // Different port and/or IP for server2
    pthread_t thread1, thread2;
    int ret1, ret2;

    // Create threads
    signal(SIGINT, handle_sigint);

    // Server setup and main loop
    
    ret1 = pthread_create(&thread1, NULL, runServer, &server1);
    if (ret1) {
        std::cerr << "Error creating thread 1: " << ret1 << std::endl;
        return 1;
    }
    ret2 = pthread_create(&thread2, NULL, runServer, &server2);
    if (ret2) {
        std::cerr << "Error creating thread 2: " << ret2 << std::endl;
        return 1;
    }
    // Wait for threads to complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
