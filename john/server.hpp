#ifndef SERVER_HPP
#define SERVER_HPP

// * CONFIG ============
#define PORT 8080

class Server
{
    public :
        int setup(size_t port) {
            if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                return -1;

            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_ANY);
            address.sin_port = htons(port);

            if((bind(serverSocket, (struct sockaddr *)&address, sizeof(address))) < 0)
                return -1;
            if (listen(serverSocket, 1000) != 0)
                perror("listen");

            sockaddr_in addrclient;
            socklen_t clientSize = sizeof(addrclient);
            clientSocket = accept(serverSocket, (sockaddr *)&addrclient, &clientSize);
            if(clientSocket)
                std::cout << "Connected !" << std::endl;
            else
            {
                perror("connect");
                return -1;
            }
            return 0;
        }

    private :
        int clientSocket;
        int serverSocket;

};


#endif