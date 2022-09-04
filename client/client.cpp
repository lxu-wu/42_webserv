#include "client.hpp"
#include <stdlib.h>

void Client::init(int i)
{
    last_time = 0;
    n_server = i;
    bzero(request, 65537);
    requestSize = 0;
}
