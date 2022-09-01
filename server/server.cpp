#include "../server/server.hpp"
#include "../parsing/requete.hpp"
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
        FD_SET(sockets[i].getServerSocket(), &readSet);
        if(sockets[i].getServerSocket() > max_fd)
            max_fd = sockets[i].getServerSocket();
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
        if(FD_ISSET(sockets[i].getServerSocket(), &_read))
        {
            Client tmp;
            tmp.last_time = 0;
            tmp.setNServer(i);
            bzero(tmp.request, 2048);
            tmp.requestSize = 0;
            tmp.setSocketClient(accept(sockets[i].getServerSocket(), (sockaddr *)&addrclient, &clientSize));
            clients.push_back(tmp);
            if(sockets[i].getServerSocket() < 0)
            {
                perror("Connect");
                exit(-1);
            }
            std::cout << colors::green << "New connection !" << std::endl;//Tim
        }
    }
}



void Server::handleRequest()
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(FD_ISSET(clients[i].getClientSocket(), &_read))
        {
            std::cout << colors::bright_cyan << "New Request ! : ";//Tim
            int Reqsize = recv(clients[i].getClientSocket() , clients[i].request + clients[i].requestSize,
                MAX_REQUEST - clients[i].requestSize, 0);
            clients[i].requestSize += Reqsize;

            Requete requete(clients[i].request);
            // if (!requete.check_tim())
            //     throw RequestErr();
            std::cout << "----REQUETE num = " << i << std::endl;
            std::cout << colors::yellow << requete.getMethod() << " " << requete.getUrl() << std::endl;
            std::cout << colors::grey << clients[i].request << std::endl;

            if(clients[i].requestSize > MAX_REQUEST)
            {
                showError(413, clients[i]);
                if(kill_client(clients[i], requete))
                    i--;
                continue;
            }
            if (Reqsize < 0)
            {
                std::cout << "Recv failed !" << std::endl;
                showError(500, clients[i]);
                kill_client(clients[i], requete);
                i--;
            }
            else if(Reqsize == 0)
            {
                std::cout << colors::on_bright_red << "Connection is closed !" << std::endl;
                kill_client(clients[i], requete);
                i--;
            }

            else
            {
                if(requete.getLen() > (size_t)stoi(servers[clients[i].getNServer()]->getBody()))
                {
                    std::cout << "Unautorised Method " << requete.getMethod() << " !" << std::endl;
                    showError(413, clients[i]);
                    if(kill_client(clients[i], requete))
                        i--;
                    continue;
                }
                if(!is_allowed(servers[clients[i].getNServer()]->getMethod(), requete.getMethod()))
                {
                    std::cout << "Unautorised Method " << requete.getMethod() << " !" << std::endl;
                    showError(405, clients[i]);
                    if(kill_client(clients[i], requete))
                        i--;
                    continue;
                }
                if (is_cgi(requete.getUrl()))
                {
                    std::cout << colors::blue << "CGI Start !" << colors::grey << std::endl;
                    std::string rescgi = execCGI(requete.getUrl(), envp, requete);
                    if(rescgi.empty())
                        showError(404, clients[i]);

                    std::cout << rescgi << std::endl;
                    rescgi = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + rescgi;
                    send(clients[i].getClientSocket(), rescgi.c_str(), rescgi.size(), 0);
                }
                // else if (clients[i].is_timeout())
                //     showError(408, clients[i]);
                else if (requete.getMethod() == "GET")
                    getMethod(clients[i], requete.getUrl().substr(1, requete.getUrl().size()));
                else if (requete.getMethod() == "POST")
                    postMethod(clients[i], requete.getUrl().substr(1, requete.getUrl().size()), requete);
                else if (requete.getMethod() == "DELETE")
                    deleteMethod(clients[i], requete.getUrl().substr(1, requete.getUrl().size()));
            }
            if(kill_client(clients[i], requete))
                i--;
            clients[i].requestSize = 0;
            bzero(clients[i].request, 2048);
        }
    }
    usleep(500);
}

void Server::getMethod(Client &client, std::string url)
{
    std::cout << colors::bright_yellow << "GET Method !" << std::endl;//Tim

    url = getRootPatch(url, client.getNServer());
    FILE *fd = fopen(url.c_str(), "rb");
    struct stat path_stat;
    stat(url.c_str(), &path_stat);
    if(fd == NULL)
    {
        std::cout << colors::on_bright_red << "ERROR: Could not open "<< url << colors::on_grey << std::endl;//Tim
        showError(404, client);
    }
    else
    {
        if(S_ISDIR(path_stat.st_mode))
        {
            std::cout << colors::on_bright_red << "File is a directory !" << colors::on_grey << std::endl;
            if(strcmp(url.c_str(), servers[client.getNServer()]->getRoot().c_str()) == 0)
                showPage(client, servers[client.getNServer()]->getIndex(), 200);
            else
                rep_listing(client.getClientSocket(), url);
        }
        else
        {
            showPage(client, url, 200);
        }
        fclose(fd);
    }
}

void Server::deleteMethod(Client &client, std::string url)
{
    std::cout << colors::bright_yellow << "DELETE Method !" << std::endl;
    url = getRootPatch(url, client.getNServer());


    FILE *fd = fopen(url.c_str(), "r");
    if(!fd)
    {
        showError(404, client);
        return ;
    }
    fclose(fd);
    std::remove(url.c_str());

    std::string tosend = "HTTP/1.1 200 OK\n";
    int ret = send(client.getClientSocket() ,tosend.c_str(), tosend.size(), 0);
    if(ret < 0)
        showError(500, client);
    else if (ret == 0)
		showError(400, client);
    std::cout << colors::green << url << " as been delete !" << std::endl;
}

void Server::postMethod(Client client, std::string url, Requete req)
{
    url = getRootPatch(url, client.getNServer());

	struct stat buf;
	lstat(url.c_str(), &buf);
    if(S_ISDIR(buf.st_mode)) {
        // if()
    }
    else
    {
        // int fd = open(url.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
        // if(fd < 0)
        // {
        //     showError(500, client);
        //     return ;
        // }
        // // ADD to queue
        // int r = write(fd, req.getBody().c_str(), req.getLen());
        // if(r < 0)
        // {
        //     showError(500, client);
        //     return ;
        // }
        // close(fd);
    }
    if(req.getLen() == 0)
        showPage(client, NULL, 201);
    else
        showPage(client, NULL, 204);
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

