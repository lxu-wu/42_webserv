#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>
#include <ctime>

#include "parsing/webserv.hpp"
#include "server/server.hpp"

void deleteallfd(Server serv)
{
    for(size_t i = 0; i < serv.getSocketList().size(); i++)
    {
        close(serv.getSocketList()[i]->getServerSocket());
    }
    for(size_t i = 0; i < serv.getClientsList().size(); i++)
    {
        close(serv.getClientsList()[i].getClientSocket());
    }
    std::cout << colors::green << "Clean All Fd" << std::endl;
}


int main(int ac, char **av)
{
    Server serv;
    Conf data;
	try
	{
		parsing(ac, av, data);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

    //data.print_all_data(); //USED BY TIM FOR TEST
    serv.info = data;
	//serv.info.print_all_data(); // ! For test

    serv.initServer();
    while(1)
    {
        serv.waitClient();
        serv.acceptClient();
        serv.handleRequest();
    }
    
    deleteallfd(serv);

}
