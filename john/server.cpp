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
    errors.insert(std::pair<int, std::string>(400, "Bad Request"));
    errors.insert(std::pair<int, std::string>(403, "Forbidden"));
    errors.insert(std::pair<int, std::string>(404, "Not Found"));
    errors.insert(std::pair<int, std::string>(400, "Bad Request"));
    errors.insert(std::pair<int, std::string>(400, "Bad Request"));
    errors.insert(std::pair<int, std::string>(400, "Bad Request"));
    errors.insert(std::pair<int, std::string>(400, "Bad Request"));
}


void Server::showError(int err)
{
    std::map<int , std::string>::iterator it = errors.find(err);
    if(it != errors.end())
    {
        std::cout << "Show error : " << err << " !" << std::endl;
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


bool Server::kill_client(Client client)
{
    close(client.getClientSocket());

    for(int i = 0; i < clients.size(); i++)
    {
        if(clients[i].getClientSocket() == client.getClientSocket())
        {
            clients.erase(clients.begin() + i);
            return true;
        }
    }
    exit(1);
}



void Server::handleRequest()
{
    for(int i = 0; i < clients.size(); i++)
    {
        if(FD_ISSET(clients[i].getClientSocket(), &_read))
        {
            std::cout << "New Request ! : ";
            int Reqsize = recv(clients[i].getClientSocket() , clients[i].request + clients[i].requestSize, 2048 - clients[i].requestSize, 0);
            clients[i].requestSize += Reqsize;

            // ! for testing =======
            std::stringstream ss;
            ss << clients[i].request;
            std::string method;
            std::string url;

            ss >> method;
            ss >> url;

            // ! =============

            std::cout << method << "   " << url << std::endl;
            std::cout << clients[i].request << std::endl;

            if(clients[i].requestSize > 2048)
            {
                std::cout << "out of range" << std::endl;
                showError(403);
                if(kill_client(clients[i]))
                    i--;
                continue;
            }
            if(Reqsize == 0)
            {
                std::cout << "Connection is closed !" << std::endl;
                kill_client(clients[i]);
                i--;
            }
            else if (Reqsize < 0)
            {
                // std::cout << "Connection is closed !" << std::endl;
                showError(403);
                kill_client(clients[i]);
                i--;
            }
            else
            {
                if (method == "GET") {
                    getMethod(clients[i], url);
                }
                else if (method == "POST") {

                }
                else if (method == "DELETE") {

                }
            }
            // showPage(clients[i].getClientSocket(), "index.html");

            if(kill_client(clients[i]))
                i--;
            clients[i].requestSize = 0;
            bzero(clients[i].request, 2048);
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

void Server::getMethod(Client &client, std::string url)
{
    std::cout << "GET Method !" << std::endl;
    FILE *fd = fopen(url.c_str(), "rb");
    if(fd == NULL)
    {
        std::cout << "ERROR: Could not open " << url << std::endl;
        exit(-1); // error 404
    }
    else
    {
        if(ISDIR(fd))
            std::cout << "File is a directory !" << std::endl;
        else
        {
            showPage(client.getClientSocket(), url);
        }
    }

}


