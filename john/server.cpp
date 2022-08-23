#include "server.hpp"
#include "tim_requete.hpp"
#include "webserv.hpp"
#include <stdlib.h>

#define NUM_SOCKS 3

// TO DO
// + Add parse url of req and root
//


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
    std::map<std::string , std::string>::iterator it2 = info.getServers()[client.getNServer()]->getError().find(std::to_string(err));
    if(it2 != info.getServers()[client.getNServer()]->getError().end())
    {
        showPage(client.getClientSocket(), it2->second);
    }
    else
    {
        std::map<int , std::string>::iterator it = errors.find(err);
        if(it != errors.end())
        {
            std::cout << colors::on_bright_red << "Show error : " << it->first << " : " << it->second << " !" << colors::on_grey << std::endl;
            std::string msg = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " + std::to_string(it->second.size()) + "\n\n" + it->second + "\n";
            send(client.getClientSocket() , msg.c_str(), msg.size(), 0);
        }
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
            tmp.setNServer(i);
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
            int Reqsize = recv(clients[i].getClientSocket() , clients[i].request + clients[i].requestSize,
                MAX_REQUEST - clients[i].requestSize, 0);
            clients[i].requestSize += Reqsize;


            if(clients[i].requestSize > MAX_REQUEST)
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
                showError(500, clients[i]);
                kill_client(clients[i]);
                i--;
            }
            else
            {
                Tim_requete requete(clients[i].request);
                if(!is_allowed(info.getServers()[clients[i].getNServer()]->getMethod(), requete.getMethod()))
                {
                    std::cout << "Unautorised Method " << requete.getMethod() << " !" << std::endl;
                    showError(405, clients[i]);
                    kill_client(clients[i]);
                    i--;
                    continue;
                }
                if (!requete.check_tim())
                    throw RequestErr();
                std::cout << colors::yellow << requete.getMethod() << " " << requete.getUrl() << std::endl;
                std::cout << colors::grey << clients[i].request << std::endl;
                if (requete.getMethod() == "GET") {
                    getMethod(clients[i], requete.getUrl().substr(1, requete.getUrl().size()));
                }
                else if (requete.getMethod() == "POST") {

                }
                else if (requete.getMethod() == "DELETE") {

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

    fseek (fd , 0 , SEEK_END);
    int lSize = ftell (fd);
    rewind (fd);

    char file[lSize];
    size_t len = fread(file, 1, lSize, fd);
    
    std::string data(file, len);
    std::string hello = "HTTP/1.1 200 OK\n" + data;
    send(socket , hello.c_str(), hello.size(), 0);
}

void Server::getMethod(Client &client, std::string url)
{
    std::cout << colors::bright_yellow << "GET Method !" << std::endl;

    url = getRootPatch(url, client.getNServer());
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
        {
            std::cout << colors::on_bright_red << "File is a directory !" << colors::on_grey << std::endl;
            if(strcmp(url.c_str(), "./") == 0)
                showPage(client.getClientSocket(), info.getServers()[client.getNServer()]->getIndex());
            else
                rep_listing(client.getClientSocket(), url);
        }
        else
        {
            showPage(client.getClientSocket(), url);
        }
    }
}

bool Server::is_allowed(std::vector<std::string> methodlist, std::string methodreq)
{
    for(int i = 0; i < methodlist.size(); i++)
    {
        if(methodlist[i] == methodreq)
            return true;
    }
    return false;
}

std::string Server::getRootPatch(std::string url, int i)
{
    // std::string root = info.getServers()[i]->getRoot();
    std::string ret = info.getServers()[i]->getRoot() + url;
    std::cout <<colors::green <<  ret << colors::grey<< std::endl;
    return ret;
}

#include <dirent.h>
void Server::rep_listing(int socket, std::string path)
{
    std::cout << colors::green << "Show Repository Listing" << std::endl;
    DIR *dir;
    struct dirent *ent;
    std::string tosend = "HTTP/1.1 200 OK\n\n<!DOCTYPE html>\n<html>\n<body>\n<h1>" + path + "</h1>\n<pre>\n";
    std::string data;


    if ((dir = opendir (path.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {

            tosend += "<a href=\""+ std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a>\n";
            std::cout << path + "    "  +  std::string(ent->d_name) << std::endl;
        }
        closedir (dir);
    }
    else
    {
        perror ("Directory listing");
        return ;
    }
    tosend += "</pre>\n</body>\n</html>\n";
    send(socket , tosend.c_str(), tosend.size(), 0);
}

