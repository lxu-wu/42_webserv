#include "cgi.hpp"

char *searchPath(char **envp)
{
    int i = 0;

    while (envp[i])
    {
        if (!strncmp("PATH=", envp[i], 5))
            return (envp[i] + 5);
        i++;
    }
    return (0);
}

std::string fileExtent(char *filePwd)
{
    size_t  i = 0;

    while (filePwd[i])
        i++;
    while(i && filePwd[i] != '.')
        i--;
    if (!strcmp(&filePwd[i], ".py"))
        return "/python2.7";
    if (!strcmp(&filePwd[i], ".pl"))
        return "/perl";
    return "\0";
}

char    *searchExec(char *filePwd, char **envp)
{
    char *envPath = searchPath(envp);

    printf("1\n");
    if (!envPath)
        return (0);

    std::string path;
    std::string token;
    std::string const exec = fileExtent(filePwd);

    if (exec == "\0")
    {
        std::cerr << "uncompatible CGI-script" << std::endl;
        return (0);
    }

    char *tmp = strdup(envPath);
    char *ret;
    printf("1\n");
    token = std::string(strtok(tmp, ":"));
    ret = 0;
printf("2\n");
    while (token.c_str())
    {
        std::cout << token << std::endl;
        path = token + exec;
        std::cout << path << std::endl;
        if (!access(path.c_str(), X_OK))
        {
            ret = strdup(path.c_str());
            if (!ret)
            {
                perror("strdup searchExec:1");
                exit(1);
            }
            free(tmp);
            return (ret);
        }
        token = std::string(strtok(0, ":"));
    }
    printf("3\n");
    free(tmp);
    return (0);
}

char **newEnv(char *filePwd, char **envp)
{
    char **my_env;

    size_t  i = 0;

    while (envp[i])
        i++;
    
    my_env = (char **)malloc(sizeof(char *) * (i + 19 + 1));
    if (!my_env)
    {
        perror("malloc newEnv");
        exit(1);
    }

    i = 0;
    while (envp[i])
    {
        my_env[i] = envp[i];
        i++;
    }
    my_env[i++] = (char*)("AUTH_TYPE=bonjour");
    // my_env[i++] = "CONTENT_TYPE=text/html";
    // my_env[i++] = "GATEWAY_INTERFACE=CGI/1.1";
    // my_env[i++] = (char *)(std::string("PATH_INFO=") + std::string(filePwd)).c_str();
    my_env[i] = 0;
    return (my_env);
}

char *execCGI(char *filePwd, char **envp)
{
    char *execPwd = searchExec(filePwd, envp);
    if (!execPwd)
    {
        return (0);
    }

    int fdOut;
    int fd[2];
    char *tab[3];

    tab[0] = execPwd;
    tab[1] = filePwd;
    tab[2] = 0;

    char **my_env;

    my_env = newEnv(filePwd, envp);

    pipe(fd);
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork()");
        exit(1);
    }
    if (pid == 0)
    {
        if (dup2(fd[1], 1) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(fd[0]);
        if (execve(tab[0], tab, my_env) == -1)
        {
            perror("execve");
            exit(1);
        }
        close(fd[1]);
        exit(0);
    }
    else
    {
        waitpid(pid, 0, 0);
        free(my_env);
        free(tab[0]);
        char *retBuff;
        struct stat statBuff;
        size_t  i;

        if (fstat(fd[0], &statBuff) == -1)
        {
            perror("fstat");
            exit(1);
        }

        retBuff = static_cast<char *>(malloc(statBuff.st_size));
        if (!retBuff)
        {
            perror("malloc execCGI");
            exit(1);
        }

        close(fd[1]);
        i = read(fd[0], retBuff, statBuff.st_size);
        if (i == -1)
        {
            perror("read");
            exit(1);
        }
        close(fd[0]);
        retBuff[i] = 0;
        return retBuff;
    }
    return 0;

}

int main(int argc, char **argv, char **envp)
{
    char * a = execCGI((char *)"test.py", envp);
    printf("1\n");
    if (a)
        std::cout << a;
    // system("leaks a.out");
    return (0);
}
