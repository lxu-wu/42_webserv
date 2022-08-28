NAME = webserv

SRCS_PATH		= ./srcs
INCS_PATH		= ./includes

SRCS =	main.cpp client/client.cpp parsing/parse_conf.cpp parsing/Classes.cpp parsing/tim_requete.cpp \
		server/server.cpp server/socket.cpp utils/utils_server.cpp

OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++
CXXFLAGS		= -Wall -Werror -Wextra -std=c++98
# CXXFLAGS		= -std=c++98 -fsanitize=address

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) -I $(INCS_PATH) -o $(NAME) $(OBJS)

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean $(NAME)

.PHONY:			all clean fclean re