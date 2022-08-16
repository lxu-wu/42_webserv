#ifndef SOCKET_HPP
#define SOCKET_HPP


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>
#include <string>  
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

class Socket
{
    public :
        void setup(size_t port); // setup the server and client socket
        void start(); // Wait for client
        void showPage(std::string dir); // Send a request to the server for show page requested
        void waitClient(); // Wait for client to connect


    private :
        void getClientInfo(); // Get first req of client
        int clientSocket;
        int serverSocket;

};


#endif