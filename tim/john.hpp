/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   john.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartial <tmartial@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 15:36:43 by tmartial          #+#    #+#             */
/*   Updated: 2022/08/15 16:19:00 by tmartial         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JOHN_HPP
# define JOHN_HPP

# include "webserv.hpp"
// * CONFIG ============
# define PORT 8080

class Server
{
    public :
        int setup(size_t port) {
            if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                return -1;

            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_ANY);
            address.sin_port = htons(port);

            if((bind(serverSocket, (struct sockaddr *)&address, sizeof(address))) < 0)
                return -1;
            if (listen(serverSocket, 1000) != 0)
                perror("listen");
            return 0;
        }

        int getInfo() // TEST
        {
            char buffer[1024] = {0};
            int valread = read( clientSocket , buffer, 1024);
            std::cout << buffer << std::endl;
            if(valread < 0)
            { 
                printf("No bytes are there to read");
            }
            // std::string hello = "Hello from the server";//IMPORTANT! WE WILL GET TO IT
            write(clientSocket , "hello" , 5);
            return 0;
        }

        void start() // Start the queue loop
        {
            sockaddr_in addrclient;
            socklen_t clientSize = sizeof(addrclient);

            while(1)
            {
                clientSocket = accept(serverSocket, (sockaddr *)&addrclient, &clientSize);
                if(clientSocket)
                    std::cout << "Connected !" << std::endl;
                else
                {
                    perror("connect");
                    exit(-1);
                }
                this->getInfo();
            }

        }

    private :
        int clientSocket;
        int serverSocket;

};


#endif