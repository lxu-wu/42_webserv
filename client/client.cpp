#include "client.hpp"
#include <stdlib.h>

void Client::init(int i)
{
    last_time = 0;
    n_server = i;
    bzero(request, 2048 + 1);
    requestSize = 0;
}
