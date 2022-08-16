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
    write = writeSet;
    read = readSet;
}


void Server::acceptClient()
{
    sockaddr_in addrclient;
    socklen_t clientSize = sizeof(addrclient);

    for(int i = 0; i < sockets.size(); i++)
    {
        if(FD_ISSET(sockets[i]->getServerSocket(), &read))
        {
            Client tmp;
            tmp.setSocketClient(accept(sockets[i]->getServerSocket(), (sockaddr *)&addrclient, &clientSize));
            clients.push_back(tmp);
            if(sockets[i]->getServerSocket())
                std::cout << "Connected on " << sockets[i]->getServerSocket() << std::endl;
            else
            {
                perror("Connect");
                exit(-1);
            }
        }
    }
}

void Server::handleRequest()
{
    for(int i = 0; i < clients.size(); i++)
    {
        // char clientrep[1024];
        // bzero(clientrep, 1024);
        // int valread = recv( clients[i].getClientSocket() , &clientrep, 1024, 0);
        // std::cout << clientrep << std::endl;

    }
}



// void Socket::getClientInfo()
// {
//     char clientrep[1024];
//     int valread = recv( clientSocket , &clientrep, 1024, 0); 
//     std::cout << clientrep << std::endl;
//     if(valread < 0)
//     {
//         std::cout << "First client request is empty" << std::endl;
//         exit(-1);
//     }
//     showPage("index.html"); // ! to change after parsing
// }

// void Socket::showPage(std::string dir)
// {
//     int fd = open(dir.c_str(), O_RDONLY);
//     char file[1024];
//     int len = read(fd, file, 1024);
//     // std::cout << file << std::endl;
    
//     std::string data(file, len);
//     std::string hello = "HTTP/1.1 200 OK\n" + data;
//     send(clientSocket , hello.c_str(), hello.size(), 0);
//     // clientSocket << fd;

// }