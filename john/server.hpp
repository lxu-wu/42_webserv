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
#include <map>
#include <sstream>
#include <sys/stat.h>

#include "webserv.hpp"
#include "tim_requete.hpp"
#include "colors.hpp"
#include "socket.hpp"
#include "client.hpp"
#include "webserv.hpp"

#define MAX_REQUEST 2048

class Server
{
    public :
        void listAllSockets(); // put all sockets in the list
        void waitClient(); // wait until client is connected
        void acceptClient(); // accept client and register them in vec
        void handleRequest(); // handle GET POST
        void showPage(int socket, std::string dir);
        void showError(int err, Client &client);
        bool kill_client(Client client);

        void getMethod(Client &client, std::string url);

        std::vector<Socket *> getSocketList() {return sockets; }
        std::vector<Client> getClientsList() {return clients; }

        Conf info;

    private :
        std::vector<Socket *> sockets;
        std::vector<Client> clients;
        std::map<int , std::string> errors;


        fd_set _read;
        fd_set _write;
};

#endif