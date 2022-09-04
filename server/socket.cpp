#include "socket.hpp"

void Socket::setup(size_t port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ServerSocket");
        exit(-1);
    }
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    // if(setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (void *)&time_start, sizeof(struct)))
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
        std::cout << colors::bright_yellow << "[" << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "]" << "[" << port << "] listening ..." << std::endl;
    }
}


