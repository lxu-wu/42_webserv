
#ifndef CGI_HPP
# define CGI_HPP

#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <stdlib.h>
#include <climits>
#include <sys/stat.h>

std::string execCGI(char *filePwd, char **envp, Tim_requete req);

#endif
