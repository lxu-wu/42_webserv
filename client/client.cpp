#include "client.hpp"

void Client::init(int i)
{
    last_time = 0;
    n_server = i;
    bzero(request, 2049);
    requestSize = 0;
}
