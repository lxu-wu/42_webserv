#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
    public :
        void setSocketClient(int sock) { clientSocket = sock; }
        int getClientSocket() { return clientSocket; }
        int requestSize;
        char request[2049];

    private :
        int clientSocket;
        

};

#endif