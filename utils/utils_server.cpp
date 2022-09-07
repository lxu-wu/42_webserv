#include "../server/server.hpp"
#include "../parsing/requete.hpp"
#include "../parsing/webserv.hpp"

void Server::initServer()
{
    for(size_t i = 0; i < servers.size(); i++)
    {
        Socket socket;
        socket.setup(stoi(servers[i]->getListen()));
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
    max_fd = -1;
}

void Server::showError(int err, Client &client)
{
    // std::map<std::string , std::string> err_map = servers[client.getNServer()]->getError();
    // std::map<std::string , std::string>::iterator it2 = err_map.find(std::to_string(err));
    // if(it2 != servers[client.getNServer()]->getError().end())
    //     showPage(client, it2->second, 200);
    // else
    // {
        std::map<int , std::string>::iterator it = errors.find(err);
        if(it != errors.end())
        {
            std::cout << colors::on_bright_red << "Show error : " << it->second << " !" << colors::on_grey << std::endl;
            std::string msg = "HTTP/1.1 " + it->second + "\nContent-Type: text/plain\nContent-Length: " + std::to_string(it->second.size()) + "\n\n" + it->second + "\n";
            int sendret = send(client.getClientSocket() , msg.c_str(), msg.size(), 0);
            if(sendret < 0)
                std::cout << "Client disconnected" << std::endl;
            else if (sendret == 0)
                std::cout << "0 byte passed to server" << std::endl;
        }
        
    // }
}


bool Server::kill_client(Client client, Requete req)
{
    (void)req; // If we take keep-alive connection
    // if(req.getHeader().find("Connection") != req.getHeader().end())
    // {
    //     std::cout << "----->" << req.getHeader().find("Connection")->second << std::endl;
    //     if(req.getHeader()["Connection"] == "close")
    //         return true;
    //     else if (req.getHeader()["Connection"] == "keep-alive")
    //         return false;
    //     showError(400, client);
    // }
    close(client.getClientSocket());
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(clients[i].getClientSocket() == client.getClientSocket())
        {
            clients.erase(clients.begin() + i);
            return true;
        }
    }
    exit(1);
}

bool Server::kill_client(Client client)
{
    std::cout << colors::red << "Client Killed" << colors::grey << std::endl;
    close(client.getClientSocket());
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(clients[i].getClientSocket() == client.getClientSocket())
        {
            clients.erase(clients.begin() + i);
            return true;
        }
    }
    exit(1);
}

std::string find_type(std::string dir)
{
    char *dot = strrchr((char *)dir.c_str(), '.');
    if(strcmp(dot, ".css") == 0) return "text/css";
    if(strcmp(dot, ".jpeg") == 0) return "image/jpeg";
    if(strcmp(dot, ".jpg") == 0) return "image/jpg";
    if(strcmp(dot, ".gif") == 0) return "image/gif";
    if(strcmp(dot, ".png") == 0) return "image/png";
    if(strcmp(dot, ".js") == 0) return "application/javascript";
    if(strcmp(dot, ".mp4") == 0) return "video/mp4 ";
    if(strcmp(dot, ".json") == 0) return "application/json";
    if(strcmp(dot, ".pdf") == 0) return "application/pdf";
    if(strcmp(dot, ".html") == 0) return "text/html";

    return "text/plain";
}

bool Server::is_allowed(std::vector<std::string> methodlist, std::string methodreq)
{
    for(size_t i = 0; i < methodlist.size(); i++)
    {
        if(methodlist[i] == methodreq)
            return true;
    }
    return false;
}

std::string Server::getRootPatch(std::string urlrcv, int i)
{
    std::string urlroot = servers[i]->getRoot();
    if(urlroot[urlroot.size() - 1] == '/')
        urlroot.erase(urlroot.size() - 1, 1);
    std::cout << (loc == NULL) << std::endl;
    if(loc && !(loc->getRoot().empty()))
        urlrcv.erase(urlrcv.find(loc->getDir()), urlrcv.find(loc->getDir()) + loc->getDir().size());



    std::cout << "Url To Send : " << colors::green << urlroot + urlrcv << std::endl;
    return urlroot + urlrcv;
}

bool Server::is_cgi(std::string filename)
{
    std::vector<std::string>  cgi_list;
    cgi_list.push_back(".py");
    cgi_list.push_back(".pl");
    if(filename.find('.') == std::string::npos)
        return false;
    std::string extension = filename.substr(filename.find('.'), filename.size());
    for(size_t i = 0; i < cgi_list.size(); i++)
    {
        if(cgi_list[i] == extension)
            return true;
    }
    return false;
}


void Server::showPage(Client client, std::string dir, int code)
{
    std::cout << colors::on_cyan << "Show Page : " << dir << colors::on_grey << std::endl;

    std::string msg;

    if(dir.empty())
        msg = "HTTP/1.1 " + errors.find(code)->second + "\n\n";
    else
    {
        FILE *fd = fopen(dir.c_str(), "rb");
        if(fd == NULL)
        {
            std::cout << colors::on_bright_red << "Error: Couldn't open " << dir << colors::on_grey << std::endl;
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
        msg = "HTTP/1.1 " + errors.find(code)->second + "\n" + "Content-Type: " + type + "\nContent-Length: " + std::to_string(lSize) + "\n\n" + data;
    }
    int ret = send(client.getClientSocket() , msg.c_str(), msg.size(), 0);
    if(ret < 0)
        showError(500, client);
    else if(ret == 0)
        showError(400, client);
}

void Server::rep_listing(int socket, std::string path, std::string fullurl, Client client)
{
    std::cout << colors::on_cyan << "Show Repository Listing" << colors::on_grey << std::endl;

    DIR *dir;
    struct dirent *ent;
    std::string data;

    std::string tosend = "HTTP/1.1 200 OK\n\n<!DOCTYPE html>\n<html>\n<body>\n<h1>" + path + "</h1>\n<pre>\n";

    if ((dir = opendir (fullurl.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            tosend += "<a href=\"" + ((std::string(ent->d_name) == ".") ? std::string(path) : (std::string(path) + "/" + std::string(ent->d_name))) + "\">" + std::string(ent->d_name) + "</a>\n";
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
    int r = send(socket , tosend.c_str(), tosend.size(), 0);
    if(r < 0)
        showError(500, client);
    else if (r == 0)
        showError(400, client);
     
}

bool Server::writewithpoll(std::string url, Client client, Requete req)
{
    int r = 0;
    int fd = open(url.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(fd < 0)
    {
        showError(500, client);
        close(fd);
        return false;
    }

    // ADD to write queue =================================
    // FD_SET(fd, &readSet);
    // if(fd > max_fd)
    //     max_fd = fd;
    // if((r = select(max_fd + 1, &readSet, &writeSet, 0, 0)) < 0)
    //     exit(-1);
    // else if (r == 0)
    //     std::cout << "Select Timeout" << std::endl;
    // if(FD_ISSET(fd, &writeSet) == 0)
    // {
    //     std::cout << "test2" << std::endl;
    //     showError(500, client);
    //     close(fd);
    //     return false;
    // }
    // =======================

    std::cout << colors::green << req.getFullBody() << std::endl;
    r = write(fd, req.getFullBody().c_str(), req.getFullBody().size()); // ! get body dont work
    if(r < 0)
    {
        showError(500, client);
        close(fd);
        return false;
    }
    close(fd);
    return true;
}

Location *Server::getLocation(std::string url, int i)
{
    std::vector<Location *> locs = servers[i]->getLocation();
    for(size_t i = 0; i < locs.size(); i++)
    {
        if(strncmp(locs[i]->getDir().c_str(), url.c_str(), locs[i]->getDir().size()) == 0)
        {
            std::cout << colors::on_cyan << url << " is Location !" << colors::on_grey << std::endl;
            return locs[i];
        }
    }
    return NULL;
}


