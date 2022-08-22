#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>
#include <ctime>

#include "webserv.hpp"
#include "server.hpp"

void deleteallfd(Server serv)
{
    for(int i = 0; i < serv.getSocketList().size(); i++)
    {
        close(serv.getSocketList()[i]->getServerSocket());
    }
    for(int i = 0; i < serv.getClientsList().size(); i++)
    {
        close(serv.getClientsList()[i].getClientSocket());
    }
}


int main(int ac, char **av)
{
    // if(ac != 2)
    // {
    //     std::cerr << "Usage : ./Webserv <config_file> " << std::endl;
    //     return 0;
    // }
    // Conf data;
	// try
	// {
	// 	parsing(ac, av, data);
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << std::endl;
	// 	return (1);
	// }
	// data.print_all_data();

    Server serv;
    serv.listAllSockets();

    while(1)
    {
        serv.waitClient();
        serv.acceptClient();
        serv.handleRequest();
    }
    // deleteallfd(serv);

	// // parsing(ac, av, data);

    // int ports[NSERV] = {8080, 8081, 8082};
    // std::vector<Server> servers(NSERV);
    // for(int i = 0; i < NSERV; i++)
    //     servers[i].setup(ports[i]);


}
