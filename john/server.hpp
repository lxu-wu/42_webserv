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
#include <netdb.h>


#include "socket.hpp"
#include "client.hpp"

class Server
{
    public :
        void listAllSockets(); // put all sockets in the list
        void waitClient(); // wait until client is connected
        void acceptClient(); // accept client and register them in vec
        void handleRequest(); // handle GET POST
        void showPage(int socket, std::string dir);
        std::vector<Socket *> getSocketList() {return sockets; }
        std::vector<Client> getClientsList() {return clients; }


    private :
        std::vector<Socket *> sockets;
        std::vector<Client> clients;


        fd_set _read;
        fd_set _write;
};

#endif