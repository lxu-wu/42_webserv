#include "Socket.hpp"

void Socket::setup(size_t port) {
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
    else
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        std::cout << "[" << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "]" << "[" << port << "] listening ..." << std::endl;
    }
}

void Socket::waitClient()
{
    fd_set readSet;
    fd_set writeSet;

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);

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
