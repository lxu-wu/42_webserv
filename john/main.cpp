#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>

#include "server.hpp"

#define PORT 8080

int main(int ac, char **av)
{
    (void)ac;
    (void)av;
    Server serv;

    serv.setup(PORT);
    // char buffer[1024] = {0};
    // int valread = read( client , buffer, 1024); 
    // std::cout << buffer << std::endl;
    // if(valread < 0)
    // { 
    //     printf("No bytes are there to read");
    // }
    // // std::string hello = "Hello from the server";//IMPORTANT! WE WILL GET TO IT
    // write(client , "hello" , 5);
    // close(client);
}
