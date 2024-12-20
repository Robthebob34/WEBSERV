#include "../include/Cgi.hpp"
Cgi::Cgi(){}

Cgi::~Cgi(){}

Cgi::Cgi(std::string path, std::string method)
{
    int i = 0;

    if(path.find_first_of('?') == std::string::npos)
        this->exec_path = path;
    else
    {
        i = path.find('?');
        this->exec_path = path.substr(0, i);
        this->query_string = path.c_str() + i + 1;
    }
    this->env["REQUEST_METHOD"] = method;
    this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->env["PATH_INFO"] = this->exec_path;
    this->env["QUERY_STRING"] = this->query_string;


    this->char_env = (char **)calloc(sizeof(char *), this->env.size() + 1);
    std::map<std::string, std::string>::const_iterator it = this->env.begin();
    for (size_t i = 0; i < this->env.size(); i++, it++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->char_env[i] = strdup(tmp.c_str());
	}
}
std::string Cgi::exec_cgi()
{
    std::string content;

    if (pipe(this->pipe_fd) == -1) {
        std::cerr << "Error: Unable to create pipe." << std::endl;
        return ""; // implementer une sortie
    }
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Error: Unable to fork." << std::endl;
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return ""; // implementer une sortie 
    }
    if (pid == 0) // processus enfant
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        std::vector<char*> args;
        args.push_back(const_cast<char*>(this->exec_path.c_str()));
        args.push_back(nullptr);
        if (execve(this->exec_path.c_str(), args.data(), this->char_env) == -1) {
            std::cerr << "Error: execve failed." << std::endl;
            exit(1);
        }
    }
    else // processus parent
    { 
        close(pipe_fd[1]);
        char cgi_buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(pipe_fd[0], cgi_buffer, sizeof(cgi_buffer) - 1)) > 0) {
            cgi_buffer[bytes_read] = '\0'; // Null-terminate the string
            std::cout << cgi_buffer << std::endl;
            content += cgi_buffer;
        }
    }
    close(pipe_fd[0]); 
    int status;
    waitpid(pid, &status, 0);
    return(content);
}