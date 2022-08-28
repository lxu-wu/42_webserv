#include "../server/server.hpp"
#include "../parsing/tim_requete.hpp"
#include "../parsing/webserv.hpp"

void Server::initServer()
{
    for(size_t i = 0; i < servers.size(); i++)
    {
        Socket *socket = new Socket;
        socket->setup(stoi(servers[i]->getListen()));
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
    std::map<std::string , std::string> err_map = servers[client.getNServer()]->getError();
    std::map<std::string , std::string>::iterator it2 = err_map.find(std::to_string(err));
    if(it2 != servers[client.getNServer()]->getError().end())
    {
        showPage(client, it2->second);
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

bool Server::kill_client(Client client)
{
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

std::string Server::getRootPatch(std::string url, int i)
{
    // std::string root = info.getServers()[i]->getRoot();
    std::string ret = servers[i]->getRoot() + url;
    std::cout <<colors::green <<  ret << colors::grey<< std::endl;
    return ret;
}