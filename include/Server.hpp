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

    // New private methods for static file serving
    void handleClient(int client_fd); // Handles incoming client requests
    std::string getFilePath(const std::string& request_path); // Gets the file path based on the request
    void serveFile(int client_fd, const std::string& file_path); // Serves the requested file
    std::string getContentType(const std::string& file_path); // Gets the content type based on the file extension
    void send404(int client_fd); // Sends a 404 Not Found response

    // Existing methods
    bool createSocket();
    bool bindSocket();
    bool makeNonBlocking();
    void acceptConnections();
};

#endif // SERVER_HPP
