#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/time.h>

class Client
{
    public :
        void setSocketClient(int sock) { clientSocket = sock; }
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

        int requestSize;
        char request[2049];
        size_t last_time;

    private :
        int clientSocket;
        int n_server;

};

#endif