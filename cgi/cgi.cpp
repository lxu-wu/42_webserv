#include "cgi.hpp"

std::string fileExtent(std::string filePwd)
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
    return "";
}

std::string searchExec(std::string filePwd, char **envp)
{
    (void)envp;
    std::string path;
    std::string token;
    std::string const exec = fileExtent(filePwd);

    if (exec == "")
    {
        std::cerr << "uncompatible CGI-script" << std::endl;
        return (0);
    }

    if (!access(exec.c_str(), X_OK))
        return exec;
    return (0);
}

std::vector<std::string> newEnv(std::string filePwd, char **envp, Requete req)
{
    std::vector<std::string> my_env;

    size_t  i = 0;

    while (envp[i])
        i++;

    i = 0;
    while (envp[i])
    {
        my_env.push_back(envp[i]);
        i++;
    }
    my_env.push_back("CONTENT_TYPE=" + req.getType());
    my_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    // my_env.push_back("PATH_TRANSLATED=" + req.getPath());
    // my_env.push_back("QUERY_STRING=" + req.getQS);//getQS pour querry string
    my_env.push_back("REMOTE_ADDR=127.0.0.1");
    my_env.push_back("REQUEST_METHOD=" + req.getMethod());
    my_env.push_back("CONTENT_LENGTH=" + std::to_string(req.getLen()));
    my_env.push_back("SERVER_SOFTWARE=" + req.getProtocol());
    my_env.push_back("SERVER_NAME=127.0.0.1");
    my_env.push_back("HTTP_ACCEPT=" + req.getHeader()["Accept:"]);
    my_env.push_back("HTTP_ACCEPT_LANGUAGE=" + req.getHeader()["Accept-Language:"]);
    my_env.push_back("HTTP_USER_AGENT=" + req.getHeader()["User-Agent:"]);
    my_env.push_back("SCRIPT_NAME=" + filePwd);
    return (my_env);
}

char **vecToTab(std::vector<std::string> vec)
{
    char **tab;
    int i = 0;

    tab = (char **)malloc(sizeof(char *) * (vec.size() + 1));
    if (!tab)
    {
        perror("malloc vecToTab");
        exit(1);
    }

    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
    {
        tab[i++] = (char *)(*it).c_str();
    }
    tab[i] = 0;
    return tab;
}

std::string execCGI(std::string filePwd, char **envp, Requete req)
{
    filePwd = "." + filePwd;
    std::string execPwd = searchExec(filePwd, envp);
    if (execPwd == "")
    {
        std::cerr << "Bad file" << std::endl;
        return (0);
    }

    int fdIn;
    int fd_in[2];
    int fd_out[2];
    char *tab[3];

    tab[0] = (char *)execPwd.c_str();
    tab[1] = (char *)filePwd.c_str();
    tab[2] = 0;

    char **my_env;

    my_env = vecToTab(newEnv(filePwd, envp, req));

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
        // if (!req.getBody().empty())
        // {
        //     write(fd_in[0], req.getBody().c_str(), req.getLen());//req.getBody ou req.getBodyComplet
        // }
        write(fd_in[1], "bon", 3);
        waitpid(pid, 0, 0);
        close(fd_in[0]);
        close(fd_in[1]);
        if (dup2(fdIn, 0) == -1)
        {
            perror("dup2");
            exit(1);
        }
        // free(my_env);
        // free(tab[0]);


        char buff[32768] = {0};
        std::string ret = "";
        int  i;




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
        std::cout << ret << "\n" ;
        return ret;
    }
    return "";

}
