// include/Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netinet/in.h>

class Server {
public:
    Server(const std::string& hostname, int port);
    ~Server();

    bool setup();
    void start();

private:
    std::string hostname;
    int port;
    int server_fd; // File descriptor for the server socket
    struct sockaddr_in address;

    bool createSocket();
    bool bindSocket();
    bool makeNonBlocking();

    void acceptConnections();
    void handleClient(int client_fd); // New method to handle client requests
};

#endif
