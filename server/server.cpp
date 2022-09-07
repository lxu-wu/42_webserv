#include "../server/server.hpp"
#include "../parsing/requete.hpp"
#include "../parsing/webserv.hpp"


// TO DO
// + Add parse url of req and root
//


bool is_request_done(char *request)
{
	char *body = strstr(request, "\r\n\r\n");
	if (!body)
		return false;
	body += 4;
	if (strnstr(request, "chunked", strlen(request) - strlen(body)))
	{
		if (strstr(body, "\r\n\r\n"))
			return true;
		return false;
	}
	else if (strnstr(request, "Content-Length", strlen(request) - strlen(body)))
	{
		if (strstr(body, "\r\n\r\n"))
			return true;
		char *start = strnstr(request, "Content-Length: ", strlen(request) - strlen(body)) + 16;
		char *end = strstr(start, "\r\n");
		char *len = strndup(start, end - start);
		int len_i = atoi(len);
		free(len);
		if ((size_t)len_i <= strlen(body))
			return true;
		return false;
	}
	else if (strnstr(request, "boundary=", strlen(request) - strlen(body)))
	{
		if (strstr(body, "\r\n\r\n"))
			return true;
		return false;
	}
	return true;
}

void Server::addtowait(int socket, fd_set *set)
{
    FD_SET(socket, set);
    if(socket > max_fd)
        max_fd = socket;
}

void Server::selectfd(fd_set *read, fd_set *write)
{
    int r = 0;
    if((r = select(max_fd + 1, read, write, 0, 0)) < 0)
        exit(-1);
    else if (r == 0)
        std::cout << "Select time out" << std::endl;
    writeSet = *write;
    readSet = *read;
}


void Server::waitClient()
{
    fd_set read;
    fd_set write;

    FD_ZERO(&read);
    FD_ZERO(&write);
    for(size_t i = 0; i < sockets.size(); i++) // Set fd of server
        addtowait(sockets[i].getServerSocket(), &read);
    for(size_t i = 0; i < clients.size(); i++) // Set fd of server
        addtowait(clients[i].getClientSocket(), &read);
    selectfd(&read, &write);

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
            if(client.getClientSocket() < 0)
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

            int Reqsize = recv(clients[i].getClientSocket(),
                clients[i].request + clients[i].requestSize,
                    MAX_REQUEST_SIZE - clients[i].requestSize, 0);

            clients[i].requestSize += Reqsize;
            std::cout << clients[i].request << std::endl;

            if(clients[i].requestSize > MAX_REQUEST_SIZE)
            {
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
            else if(is_request_done(clients[i].request))
            {
                Requete requete(clients[i].request);
                // if(requete.err != -1)
                // {
                //     showError(requete.err, clients[i]);
                //     if(kill_client(clients[i], requete))
                //         i--;
                //     continue;       
                // }
                int ret = -1;
                if ((ret = requete.check_tim()) != -1)
                {
                    showError(ret, clients[i]);
                    if(kill_client(clients[i], requete))
                        i--;
                    continue;       
                }
                std::string urlrcv = requete.getUrl();


                if(requete.getLen() != std::string::npos && requete.getLen() > (size_t)stoi(servers[clients[i].getNServer()]->getBody()))
                {
                    showError(413, clients[i]);
                    if(kill_client(clients[i], requete))
                        i--;
                    continue;
                }
                
                loc = getLocation(urlrcv, clients[i].getNServer());

                if(!((loc == NULL) ? is_allowed(servers[clients[i].getNServer()]->getMethod(), \
                    requete.getMethod()) : is_allowed(loc->getMethod(), requete.getMethod())))
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

                    std::string urlsend = getRootPatch(urlrcv, clients[i].getNServer());
                    std::string rescgi = execCGI(urlsend, envp, requete);
                    if(rescgi.empty())
                        showError(404, clients[i]);

                    std::cout << rescgi << std::endl;
                    rescgi = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + rescgi;
                    int r = send(clients[i].getClientSocket(), rescgi.c_str(), rescgi.size(), 0);
                    if(r < 0)
                        showError(500, clients[i]);
                    else if (r == 0)
                        showError(400, clients[i]);
                }
                else
                {
                    // else if (clients[i].is_timeout())
                    //     showError(408, clients[i]);
                    if (requete.getMethod() == "GET")
                        getMethod(clients[i], urlrcv);
                    else if (requete.getMethod() == "POST")
                        postMethod(clients[i], urlrcv, requete);
                    else if (requete.getMethod() == "DELETE")
                        deleteMethod(clients[i], urlrcv);
                }
                if(kill_client(clients[i], requete))
                    i--;
                if(i <= 0)
                {
                    clients[i].requestSize = 0;
                    bzero(clients[i].request, MAX_REQUEST_SIZE);
                }
            }
        }
    }
    usleep(500);
}

void Server::getMethod(Client &client, std::string urlrcv)
{
    std::cout << colors::bright_yellow << "GET Method !" << std::endl;

    if(urlrcv.size() >= 64)
    {
        showError(414, client);
        return ;
    }
    struct stat path_stat;

    std::string urlsend = getRootPatch(urlrcv, client.getNServer());

    std::cout << urlsend << "   " << urlrcv << std::endl;

    if(loc && !(loc->getIndex().empty()) && (strcmp(urlrcv.c_str(), \
        loc->getDir().c_str()) == 0))
    {
        showPage(client, loc->getRoot() + loc->getIndex(), 200);
        return ;
    }

    FILE *fd = fopen(urlsend.c_str(), "rb");
    stat(urlsend.c_str(), &path_stat);

    if(fd == NULL)
    {
        std::cout << colors::on_bright_blue << "Ressource not find : "<< urlsend << colors::on_grey << std::endl;
        showError(404, client);
    }
    else
    {
        if(S_ISDIR(path_stat.st_mode))
        {
            std::cout << colors::on_bright_blue << "File is a directory !" << colors::on_grey << std::endl;
            std::cerr << urlrcv.c_str() << " vs " << servers[client.getNServer()]->getRoot().c_str() << std::endl;

            std::cout << "----->" << urlrcv << std::endl;
            if(strcmp(urlrcv.c_str(), "/") == 0)
                showPage(client, urlsend + servers[client.getNServer()]->getIndex(), 200);
            else
                rep_listing(client.getClientSocket(), urlrcv, urlsend, client);
        }
        else
            showPage(client, urlsend, 200);
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
    std::string urlsend = getRootPatch(url, client.getNServer());
	struct stat buf;
	lstat(url.c_str(), &buf);

    // OK BUT need body boundary
    if(S_ISDIR(buf.st_mode))
    {
        if(req.getHeader().find("Content-Type") != req.getHeader().end() && req.getHeader()["Content-Type"].find("boundary=") != std::string::npos)
        {
            std::cout << "Upload in directory" << std::endl;
            std::cout << req.getText().find("name")->second << std::endl;
            // for(int i = 0; i < )
            // {

            // }
        }
        else
        {
            showError(400, client);
            return ;
        }
    }
    else
    {
        if(!writewithpoll(urlsend, client, req))
            return ;
    }
    if(req.getLen() == 0)
        showPage(client, "", 204);
    else
        showPage(client, "", 201);
}

