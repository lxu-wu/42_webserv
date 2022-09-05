#include "../server/server.hpp"
#include "../parsing/requete.hpp"
#include "../parsing/webserv.hpp"


// TO DO
// + Add parse url of req and root
//

void Server::waitClient()
{
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
}


void Server::acceptClient()
{
    sockaddr_in addrclient;
    socklen_t clientSize = sizeof(addrclient);

    for(size_t i = 0; i < sockets.size(); i++)
    {
        if(FD_ISSET(sockets[i].getServerSocket(), &readSet))
        {
            Client client;
            client.init(i);
            client.setSocketClient(accept(sockets[i].getServerSocket(), (struct sockaddr *)&addrclient, &clientSize));
            clients.push_back(client);
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
        if(FD_ISSET(clients[i].getClientSocket(), &readSet))
        {
            std::cout << colors::bright_cyan << "New Request ! : ";

            int Reqsize = recv(clients[i].getClientSocket() , clients[i].request + clients[i].requestSize, 65536, 0);

            std::cout << Reqsize << std::endl;
            
            clients[i].requestSize += Reqsize;
            Requete requete(clients[i].request);
            int ret = -1;
            if ((ret = requete.check_tim()) != -1)
            {
                showError(ret, clients[i]);
                if(kill_client(clients[i], requete))
                    i--;
                continue;       
            }

            std::cout << colors::yellow << requete.getMethod() << " " << requete.getUrl() << std::endl;
            std::cout << colors::grey << clients[i].request << std::endl;

            if(clients[i].requestSize - requete.getLen() > MAX_REQUEST)
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
            bzero(clients[i].request, 65537);
        }
    }
    usleep(500);
}

void Server::getMethod(Client &client, std::string url)
{
    std::cout << colors::bright_yellow << "GET Method !" << std::endl;

    if(url.size() >= 64)
    {
        showError(414, client);
        return ;
    }
    struct stat path_stat;

    std::string fullurl = getRootPatch(url, client.getNServer());
    FILE *fd = fopen(fullurl.c_str(), "rb");
    stat(fullurl.c_str(), &path_stat);

    if(fd == NULL)
    {
        std::cout << colors::on_bright_blue << "Ressource not find : "<< fullurl << colors::on_grey << std::endl;
        showError(404, client);
    }
    else
    {
        if(S_ISDIR(path_stat.st_mode))
        {
            std::cout << colors::on_bright_blue << "File is a directory !" << colors::on_grey << std::endl;
            // std::cerr << fullurl.c_str() << " vs " << servers[client.getNServer()]->getRoot().c_str() << std::endl;

            if(strcmp(fullurl.c_str(), servers[client.getNServer()]->getRoot().c_str()) == 0)
                showPage(client, fullurl + servers[client.getNServer()]->getIndex(), 200);
            else
                rep_listing(client.getClientSocket(), url, fullurl);
        }
        else
            showPage(client, fullurl, 200);
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

    if(req.getHeader()["Transfer-Encoding"] == "chunked")
    {
        showError(411, client);
        return;
    }
    url = getRootPatch(url, client.getNServer());
	struct stat buf;
	lstat(url.c_str(), &buf);

    if(S_ISDIR(buf.st_mode)) {\
        if(req.getHeader().find("Content-Type") != req.getHeader().end())
        {
            std::cout << "Upload in directory" << std::endl;
            // while()
            // {

            // }
        }
    }
    else
    {
        int fd = open(url.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
        if(fd < 0)
        {
            showError(500, client);
            return ;
        }
        std::cerr << "Body --->" << req.getBody() << std::endl;
        int r = write(fd, req.getBody().c_str(), req.getBody().size()); // ! get body dont work
        if(r < 0)
        {
            showError(500, client);
            close(fd);
            return ;
        }
        close(fd);
    }
    if(req.getLen() == 0)
        showPage(client, "", 204);
    else
        showPage(client, "", 201);
}
