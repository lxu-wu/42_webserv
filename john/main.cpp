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

/*

--- server 0:
name = hello
listen = 4040
root = ./www/
index = index.html
body = 9000000
methods = POST GET DELETE 
error pages:
error 404 = error404.html
error 505 = error404.html
error 606 = error405.html
- location 0:
dir = /loc
root = ./website/bla
index = foo.html
methods = GET 
- location 1:
dir = /loc2
root = ./www/
index = foo.html
methods = GET 
--- server 1:
name = hello
listen = 4041
root = ./
index = index.html
body = 900000
methods = POST GET DELETE 
error pages:
error 404 = error404.html
error 505 = error404.html
error 606 = error606.html
- location 0:
dir = /loc3
root = ./website/bla
index = foo.html
methods = GET 

*/

int main(int ac, char **av)
{
    if(ac != 2)
    {
        std::cerr << "Usage : ./Webserv <config_file> " << std::endl;
        return 0;
    }
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
    serv.servers = data.getServers();

    serv.initServer();
    while(1)
    {
        serv.waitClient();
        serv.acceptClient();
        serv.handleRequest();
    }
    
    deleteallfd(serv);

}
