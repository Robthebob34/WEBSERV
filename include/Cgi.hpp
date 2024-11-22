#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <unistd.h>

class Cgi {
    private :
        std::map<std::string, std::string>	env;
        std::string exec_path;
        std::string query_string;
        char **char_env;
        int pipe_fd[2];
    public:
        Cgi();
        Cgi(std::string path, std::string method);
        ~Cgi();

        std::string    exec_cgi();

};

#endif