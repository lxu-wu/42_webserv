#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>
#include <string>  
#include <fcntl.h>
#include <stdio.h>
#include <vector>
#include <iostream>

#include "socket.hpp"
#include "Client.hpp"

class Server
{
    public :
        void listAllSockets(); // put all sockets in the list
        void waitClient(); // wait until client is connected
        void acceptClient(); // accept client and register them in vec
        void handleRequest(); // handle GET POST
        

    private :
        std::vector<Socket *> sockets;
        std::vector<Client> clients;


        fd_set read;
        fd_set write;
};

#endif