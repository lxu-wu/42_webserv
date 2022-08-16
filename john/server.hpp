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

#include "Socket.hpp"


class Server
{
    public :
        void listAllSockets(); // put all sockets in the list
        void waitClient(); // wait until client is connected



    private :
        std::vector<Socket *> sockets;
};

#endif