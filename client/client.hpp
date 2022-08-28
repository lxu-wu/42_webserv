#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
    public :
        void setSocketClient(int sock) { clientSocket = sock; }
        int getClientSocket() { return clientSocket; }
        int getNServer() { return n_server; }
        void setNServer(int n) { n_server = n; }

        int requestSize;
        char request[2049];

    private :
        int clientSocket;
        int n_server;

};

#endif