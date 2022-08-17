#include "server.hpp"
#define NUM_SOCKS 3

int ports[NUM_SOCKS] = {8080, 8081, 8082};
std::string allow[2] = {
    "GET", "POST"
};


void Server::listAllSockets()
{
    for(int i = 0; i < NUM_SOCKS; i++)
    {
        Socket *socket = new Socket;
        socket->setup(ports[i]);
        sockets.push_back(socket);
    }
}


void Server::waitClient()
{
    fd_set readSet;
    fd_set writeSet;
    int max_fd = 0;

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    for(int i = 0; i < sockets.size(); i++) // Set fd of server
    {
        FD_SET(sockets[i]->getServerSocket(), &readSet);
        if(sockets[i]->getServerSocket() > max_fd)
            max_fd = sockets[i]->getServerSocket();
    }
    for(int i = 0; i < clients.size(); i++) // Set fd of server
    {
        FD_SET(clients[i].getClientSocket(), &readSet);
        if(clients[i].getClientSocket() > max_fd)
            max_fd = clients[i].getClientSocket();
    }
    if(select(max_fd + 1, &readSet, &writeSet, 0, 0) < 0)
        exit(-1);
    _write = writeSet;
    _read = readSet;
}


void Server::acceptClient()
{
    sockaddr_in addrclient;
    socklen_t clientSize = sizeof(addrclient);

    for(int i = 0; i < sockets.size(); i++)
    {
        if(FD_ISSET(sockets[i]->getServerSocket(), &_read))
        {
            Client tmp;
            tmp.setSocketClient(accept(sockets[i]->getServerSocket(), (sockaddr *)&addrclient, &clientSize));
            clients.push_back(tmp);
            if(sockets[i]->getServerSocket() < 0)
            {
                perror("Connect");
                exit(-1);
            }
            std::cout << "New connection !" << std::endl;
        }
    }
}

// std::string test[3] = {"index.html", "www/html/index.html", "www/html/error/error.html"};

void Server::handleRequest()
{
    for(int i = 0; i < clients.size(); i++)
    {
        if(FD_ISSET(clients[i].getClientSocket(), &_read))
        {
            std::cout << "New Request !" << std::endl;
            int valread = recv(clients[i].getClientSocket() , &clients[i].getRequestBuffer(), 2048, 0);
            // showPage(clients[i].getClientSocket(),"index.html");
        }
    }
    usleep(500);
}

void Server::showPage(int socket ,std::string dir)
{
    int fd = open(dir.c_str(), O_RDONLY);
    if(fd < 0)
        return;
    char file[2048];
    int len = read(fd, file, 2048);
    // std::cout << file << std::endl;
    
    std::string data(file, len);
    std::string hello = "HTTP/1.1 200 OK\n" + data;
    send(socket , hello.c_str(), hello.size(), 0);

}
