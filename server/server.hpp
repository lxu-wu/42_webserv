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
#include <stdlib.h>

#include "../parsing/webserv.hpp"
#include "../parsing/requete.hpp"
#include "../utils/colors.hpp"
#include "../client/client.hpp"
#include "../cgi/cgi.hpp"
#include "socket.hpp"

#define MAX_REQUEST 2048

class Server
{
    public :
        void initServer(); // put all sockets in the list
        void waitClient(); // wait until client is connected
        void acceptClient(); // accept client and register them in vec
        void handleRequest(); // handle GET POST
        void showPage(Client client, std::string dir);
        void showError(int err, Client &client);
        bool kill_client(Client client);
        bool is_allowed(std::vector<std::string> methodlist, std::string methodreq); // say if a method is allowed
        void rep_listing(int socket, std::string path);
        bool is_cgi(std::string filename);


        std::string getRootPatch(std::string url, int i);
        void getMethod(Client &client, std::string url);
        void deleteMethod(Client &client, std::string url);
        void postMethod(Client &client, std::string url);

        std::vector<Socket *> getSocketList() {return sockets; }
        std::vector<Client> getClientsList() {return clients; }


        std::vector<Servers*> servers;

        char **envp;

    private :
        std::vector<Socket *> sockets;
        std::vector<Client> clients;
        std::map<int , std::string> errors;



        fd_set _read;
        fd_set _write;
};

std::string find_type(std::string dir);

#endif