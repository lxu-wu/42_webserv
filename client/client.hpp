#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/time.h>
#include <strings.h>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>


class Client
{
    public :
        void setSocketClient(int sock) {
            clientSocket = sock;

            fcntl(clientSocket, F_SETFL, O_NONBLOCK);
            // if(setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, NULL, 0) < 0)
            //     return ;
            
        }
        int getClientSocket() { return clientSocket; }
        int getNServer() { return n_server; }
        void setNServer(int n) { n_server = n; }
        bool is_timeout() { 
            static timeval tv;
            gettimeofday(&tv, NULL);
            if(tv.tv_sec - last_time > 60)
                return true;
            last_time = tv.tv_sec;
            return false;
        }

        void init(int i);

        int requestSize;
        char request[2048 + 1];
        size_t last_time;

    private :
        int clientSocket;
        int n_server;

};

#endif