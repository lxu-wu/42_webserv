#include "socket.hpp"

void Socket::setup(char *port) {
	struct addrinfo hints, *info;

	std::cout << "> Get address informations...\n";
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo("0.0.0.0", port, &hints, &info) >= 0)
	{
		std::cout << "> Creating socket...\n";
		int new_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		fcntl(new_socket, F_SETFL, O_NONBLOCK);
		if (new_socket < 0)
		{
			exit(1);
		}
		std::cout << "> Binding socket to local address...\n";
		int opt = 1;
		if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
			fprintf(stderr, "setsockopt: setsocketopt in server_socket failed\n");
		if (bind(new_socket, info->ai_addr, info->ai_addrlen))
		{
			perror("bind");
			exit(1);
		}
		freeaddrinfo(info);
		std::cout << "> Listening...\n";
		if (listen(new_socket, 10) < 0)
		{
			exit(1);
		}
		this->serverSocket = new_socket;
	}
	else
	{
		exit(1);
	}
}


