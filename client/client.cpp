#include "client.hpp"
#include <stdlib.h>

void Client::init(int i)
{
    last_time = 0;
    n_server = i;
    request = (char *)malloc(sizeof(char) * (65536 + 1));
    bzero(request, 65536 + 1);
    requestSize = 0;
}
