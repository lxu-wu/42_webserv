#include <sys/types.h> /* Voir NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>

// * CONFIG
#define PORT 8080

int main(int ac, char **av)
{
    (void)ac;
    (void)av;
    int server_fd;
    struct sockaddr_in address;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if((server_fd = bind(server_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
        return -1;
s
    if (listen(server_fd, 3) != 0)
        perror("listen");
    // int new_socket;
    // int addlen;
    // if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addlen))<0)
    //     return -1;

    // send(new_socket, "Hello, world!\n", 13, 0);
    // while(1);
}
