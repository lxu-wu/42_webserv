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
        return "/usr/bin/python2.7";
    if (!strcmp(&filePwd[i], ".pl"))
        return "/usr/bin/perl";
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

    token = std::string(strtok(tmp, ":"));
    ret = 0;
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

std::string execCGI(char *filePwd, char **envp, Tim_requete req)
{
    char *execPwd = searchExec(filePwd, envp);
    if (!execPwd)
    {
        return (0);
    }

    int fdIn;
    int fd_in[2];
    int fd_out[2];
    char *tab[3];

    tab[0] = execPwd;
    tab[1] = filePwd;
    tab[2] = 0;

    char **my_env;

    my_env = newEnv(filePwd, envp);

    pipe(fd_in);
    pipe(fd_out);
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork()");
        exit(1);
    }



    if (pid == 0)
    {
        if (dup2(fd_in[0], 0) == -1)
        {
            perror("dup2");
            exit(1);
        }
        if (dup2(fd_out[1], 1) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(fd_out[0]);
        close(fd_in[1]);
        execve(tab[0], tab, my_env);
        perror("execve");
        close(fd_out[1]);
        close(fd_in[0]);
        exit(1);
    }




    else
    {
        fdIn = dup(0);
        if (fdIn == 0)
        {
            perror("dup");
            exit(1);
        }
        if (dup2(fd_in[0], 0) == -1)
        {
            perror("dup2");
            exit(1);
        }
        if (req.getBody())
        {
            write(fd_in[0], req.getLeijie(), req.getLen());//req.getBody ou req.getLeijie
        }
        waitpid(pid, 0, 0);
        close(fd_in[0]);
        close(fd_in[1]);
        if (dup2(fdIn, 0) == -1)
        {
            perror("dup2");
            exit(1);
        }
        free(my_env);
        free(tab[0]);


        char buff[32768] = {0};
        std::string ret = "";
        size_t  i;




        close(fd_out[1]);
        i = read(fd_out[0], buff, 32767);
        if (i == -1)
        {
            perror("read");
            exit(1);
        }
        ret += std::string(buff);
        while (i < 0)
        {
            i = read(fd_out[0], buff, 32767);
            if (i == -1)
            {
                perror("read");
                exit(1);
            
            }
            buff[i] = 0;
            ret += std::string(buff);
        }
        close(fd_out[0]);
        return ret;
    }
    return "";

}

int main(int argc, char **argv, char **envp)
{
    std::string a = execCGI((char *)"test.py", envp, req);
    printf("1\n");
    if (a)
        std::cout << a;
    // system("leaks a.out");
    return (0);
}
