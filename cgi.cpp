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

char    *searchExec(char *filePwd, char **envp)
{
    char *envPath = searchPath(envp);

    if (!envPath)
        return (0);

    std::string path;
    std::string token;
    std::string exec = "/python";
    char *tmp = strdup(envPath);

    token = std::string(strtok(tmp, ":"));
    free(tmp);
    tmp = 0;

    while (token.c_str())
    {
        path = token + exec;
        if (!access(path.c_str(), X_OK))
        {
            tmp = strdup(path.c_str());
            if (!tmp)
            {
                perror("strdup searchExec:1");
                exit(1);
            }
            return (tmp);
        }
        token = std::string(strtok(0, ":"));
    }
    path = filePwd;
    if (!access(path.c_str(), X_OK))
    {
        tmp = strdup(path.c_str());
        if (!tmp)
        {
            perror("strdup searchExec:2");
            exit(1);
        }
        return (tmp);
    }
    return (0);
}

// char **newEnv(char **envp)
// {
//     char **my_env;

//     size_t  i = 0;

//     while (envp[i])
//         i++;
    
//     my_env = malloc(sizeof(char *) * (i + 19 + 1));
//     if (!my_env)
//     {
//         perror("malloc newEnv");
//         exit(1);
//     }

//     i = 0;
//     while (envp[i])
//     {
//         my_env[i] = envp[i];
//         i++;
//     }

// }

char *execCGI(char *filePwd, char **envp)
{
    char *execPwd = searchExec(filePwd, envp);
    if (!execPwd)
        return (0);

    int fdOut;
    int fd[2];
    char *tab[3];

    tab[0] = execPwd;
    tab[1] = filePwd;
    tab[2] = 0;

    char **my_env;

    for(int i = 0; tab[i];i++)
        printf("%s\n", tab[i]);
    // my_env = newEnv(envp);

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
        if (execve(tab[0], tab, 0) == -1)
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
    std::cout << execCGI((char *)"test.py", envp);
    system("leaks a.out");
    return (0);
}
