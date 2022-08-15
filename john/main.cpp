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
    serv.start();
}
