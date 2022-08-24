#include "../server/server.hpp"
#include "../parsing/tim_requete.hpp"
#include "../parsing/webserv.hpp"
#include <stdlib.h>
#include <dirent.h>

// TO DO
// + Add parse url of req and root
//

void Server::waitClient()
{
    fd_set readSet;
    fd_set writeSet;
    int max_fd = 0;

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    for(size_t i = 0; i < sockets.size(); i++) // Set fd of server
    {
        FD_SET(sockets[i]->getServerSocket(), &readSet);
        if(sockets[i]->getServerSocket() > max_fd)
            max_fd = sockets[i]->getServerSocket();
    }
    for(size_t i = 0; i < clients.size(); i++) // Set fd of server
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

    for(size_t i = 0; i < sockets.size(); i++)
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



void Server::handleRequest()
{
    for(size_t i = 0; i < clients.size(); i++)
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
            if (Reqsize < 0)
            {
                std::cout << "Recv failed !" << std::endl;
                showError(500, clients[i]);
                kill_client(clients[i]);
                i--;
            }
            else if(Reqsize == 0)
            {
                std::cout << colors::on_bright_red << "Connection is closed !" << std::endl;
                kill_client(clients[i]);
                i--;
            }
            else
            {
                Tim_requete requete(clients[i].request);
                std::cout << colors::yellow << requete.getMethod() << " " << requete.getUrl() << std::endl;
                std::cout << colors::grey << clients[i].request << std::endl;



                if(10 > stoi(info.getServers()[clients[i].getNServer()]->getBody())) // ! change value
                {
                    // std::cout << "Unautorised Method " << requete.getMethod() << " !" << std::endl;
                    showError(413, clients[i]);
                    kill_client(clients[i]);
                    i--;
                    continue;
                }
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


void Server::showPage(Client client, std::string dir)
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

    std::string type = find_type(dir);

    char file[lSize];
    size_t len = fread(file, 1, lSize, fd);
    fclose(fd);
    std::string data(file, len);
    std::string hello = std::string("HTTP/1.1 200 OK\n") + "Content-Type: " + type + "\nContent-Length: " + std::to_string(lSize) + "\n\n" + data;
    int ret = send(client.getClientSocket() , hello.c_str(), hello.size(), 0);
    if(ret < 0)
    {
        showError(500, client);
        return ;
    }
    else if(ret == 0)
    {
        showError(400, client);
        return ;
    }
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
                showPage(client, info.getServers()[client.getNServer()]->getIndex());
            else
                rep_listing(client.getClientSocket(), url);
        }
        else
        {
            showPage(client, url);
        }
        fclose(fd);
    }
}


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

