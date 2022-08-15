#include "server.hpp"

void Server::setup(size_t port) {
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ServerSocket");
        exit(-1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if((bind(serverSocket, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        perror("bind");
        exit(-1);
    }
    if (listen(serverSocket, 1000) != 0)
    {
        perror("Listenning");
        exit(-1);
    }


}

void Server::start()
{
    sockaddr_in addrclient;
    socklen_t clientSize = sizeof(addrclient);

    while(1)
    {
        clientSocket = accept(serverSocket, (sockaddr *)&addrclient, &clientSize);
        if(clientSocket)
            std::cout << "Connected !" << std::endl;
        else
        {
            perror("connect");
            exit(-1);
        }
        getClientInfo();
        close(clientSocket);
    }

}

void Server::getClientInfo()
{
    char buffer[1024] = {0};
    int valread = read( clientSocket , buffer, 1024); 
    std::cout << buffer << std::endl;
    if(valread < 0)
    {
        std::cout << "First client request is empty" << std::endl;
        exit(-1);
    }
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    write(clientSocket , &hello , hello.size() );
}