NAME =	Server
SRCS =	main.cpp parse_conf.cpp Classes.cpp

OBJ = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g
CXX = c++
rm = rm -rf

all : $(NAME)

$(NAME):	$(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: re bonus clean fclean all