#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
    public :
        void setSocketClient(int sock) { clientSocket = sock; }
        int getClientSocket() { return clientSocket; }

    private :
        int clientSocket;
        

};

#endif