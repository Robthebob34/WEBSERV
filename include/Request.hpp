#ifndef REQUEST_HPP
# define REQUEST_HPP


#include "Webserv.hpp"

class Request
{
    public:
        std::string method;
        std::string request;
        std::string FilePath;
        std::string connexion;
		ssize_t bytes_read;
        std::vector<unsigned char> data;
		char *buffer;

        Request(std::string method, std::string request);
        void    setFilePath(std::string FilePath);
        void    setConnexion(std::string Connexion);
        ~Request();
        Request();
    private:

};

#endif