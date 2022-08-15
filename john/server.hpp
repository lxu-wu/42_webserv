#ifndef SERVER_HPP
#define SERVER_HPP


#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>
#include <string>  
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

class Server
{
    public :
        void setup(size_t port); // setup the server and client socket
        void start(); // Wait for client
        void showPage(std::string dir); // Send a request to the server for show page requested

    private :
        void getClientInfo(); // Get first req of client
        int clientSocket;
        int serverSocket;

};


#endif