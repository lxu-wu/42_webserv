#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>
#include <ctime>

#include "server.hpp"
// #include "Socket.hpp"
#include "webserv.hpp"

// * CONFIG ============
// #define PORT 8080
#define NSERV 3


int main(int ac, char **av)
{
    // if(ac != 2)
    // {
    //     std::cerr << "Usage : ./Webserv <config_file> " << std::endl;
    //     return 0;
    // }
    Server serv;
    serv.listAllSockets();

    while(1)
    {
        serv.waitClient();
        serv.acceptClient();
        serv.handleRequest();
    }


	// // parsing(ac, av, data);

    // int ports[NSERV] = {8080, 8081, 8082};
    // std::vector<Server> servers(NSERV);
    // for(int i = 0; i < NSERV; i++)
    //     servers[i].setup(ports[i]);



}
