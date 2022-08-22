#include "server.hpp"
#include <stdlib.h>

#define NUM_SOCKS 3

int ports[NUM_SOCKS] = {8080, 8081, 8082};
std::string allow[2] = {
    "GET", "POST"
};


void Server::listAllSockets()
{
    for(int i = 0; i < info.getServers().size(); i++)
    {
        Socket *socket = new Socket;
        socket->setup(stoi(info.getServers()[i]->getListen()));
        sockets.push_back(socket);
    }
	errors.insert(std::make_pair(200, "200 OK"));
	errors.insert(std::make_pair(201, "201 Created"));
	errors.insert(std::make_pair(204, "204 No Content"));
	errors.insert(std::make_pair(300, "300 Multiple Choices"));
	errors.insert(std::make_pair(301, "301 Moved Permanently"));
	errors.insert(std::make_pair(302, "302 Found"));
	errors.insert(std::make_pair(303, "303 See Other"));
	errors.insert(std::make_pair(307, "307 Temporary Redirect"));
	errors.insert(std::make_pair(400, "400 Bad Request"));
	errors.insert(std::make_pair(404, "404 Not Found"));
	errors.insert(std::make_pair(405, "405 Method Not Allowed"));
	errors.insert(std::make_pair(408, "408 Request Timeout"));
	errors.insert(std::make_pair(411, "411 Length Required"));
	errors.insert(std::make_pair(413, "413 Request Entity Too Large"));
	errors.insert(std::make_pair(414, "414 Request-URI Too Long"));
	errors.insert(std::make_pair(500, "500 Internal Server Error"));
	errors.insert(std::make_pair(502, "502 Bad Gateway"));
	errors.insert(std::make_pair(505, "505 HTTP Version Not Supported"));
}


void Server::showError(int err, Client &client)
{
    std::map<int , std::string>::iterator it = errors.find(err);
    if(it != errors.end())
    {
        std::cout << colors::on_bright_red << "Show error : " << it->first << " : " << it->second << " !" << colors::on_grey << std::endl;
        std::string msg = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " + std::to_string(it->second.size()) + "\n\n" + it->second + "\n";
        send(client.getClientSocket() , msg.c_str(), msg.size(), 0);
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

            bzero(tmp.request, 2048);
            tmp.requestSize = 0;
            tmp.setSocketClient(accept(sockets[i]->getServerSocket(), (sockaddr *)&addrclient, &clientSize));
            clients.push_back(tmp);
            if(sockets[i]->getServerSocket() < 0)
            {
                perror("Connect");
                exit(-1);
            }
            std::cout << colors::green << "New connection !" << std::endl;
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
            std::cout << colors::bright_cyan << "New Request ! : ";
            int Reqsize = recv(clients[i].getClientSocket() , clients[i].request + clients[i].requestSize, MAX_REQUEST - clients[i].requestSize, 0);
            clients[i].requestSize += Reqsize;

            std::cout << clients[i].requestSize << std::endl;

            // ! for testing =======
            std::stringstream ss;
            ss << clients[i].request;
            std::string method;
            std::string url;

            ss >> method;
            ss >> url;
            method = "GET";

            // ! =============

            std::cout << colors::yellow << method << " " << url << std::endl;
            std::cout << colors::grey << clients[i].request << std::endl;

            if(clients[i].requestSize > 2048)
            {
                std::cout << colors::on_bright_red << "out of range" << std::endl;
                showError(413, clients[i]);
                if(kill_client(clients[i]))
                    i--;
                continue;
            }
            if(Reqsize == 0)
            {
                std::cout << colors::on_bright_red << "Connection is closed !" << std::endl;
                kill_client(clients[i]);
                i--;
            }
            else if (Reqsize < 0)
            {
                std::cout << "Connection is closed !" << std::endl;
                showError(413, clients[i]);
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
            if(kill_client(clients[i]))
                i--;
            clients[i].requestSize = 0;
            bzero(clients[i].request, 2048);
        }
    }
    usleep(500);
}

void Server::showPage(int socket, std::string dir)
{
    FILE *fd = fopen(dir.c_str(), "rb");
    if(fd == NULL)
    {
        std::cout << colors::on_bright_red << "Error: Couldn't open " << dir << std::endl;
        return ;
    }
    char file[2048];
    size_t len = fread(file, 1, 2048, fd);
    // std::cout << file << std::endl;
    
    std::string data(file, len);
    std::string hello = "HTTP/1.1 200 OK\n" + data;
    send(socket , hello.c_str(), hello.size(), 0);

}

void Server::getMethod(Client &client, std::string url)
{
    url = url.substr(1, url.end() - url.begin());
    std::cout << colors::bright_yellow << "GET Method !" << std::endl;
    FILE *fd = fopen(url.c_str(), "rb");
    struct stat path_stat;
    stat(url.c_str(), &path_stat);
    if(fd == NULL)
    {
        std::cout << colors::on_bright_red << "ERROR: Could not open "<< url << colors::on_grey << std::endl;
        showError(404, client);
    }
    else
    {
        if(S_ISDIR(path_stat.st_mode))
            std::cout << colors::on_bright_red << "File is a directory !" << colors::on_grey << std::endl;
        else
        {
            showPage(client.getClientSocket(), url);
        }
    }

}


