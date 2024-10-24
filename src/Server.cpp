#include "../include/Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <fstream> // Add this line at the top of your Server.cpp
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>

Server::Server(const std::string& hostname, int port) : hostname(hostname), port(port), server_fd(-1) {}

Server::~Server() {
    if (server_fd != -1) {
        close(server_fd); // Close the socket when the server is destroyed
    }
}

bool Server::setup() {
    return createSocket() && bindSocket() && makeNonBlocking();
}

bool Server::createSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error: Failed to create socket." << std::endl;
        return false;
    }
    return true;
}

// bool Server::bindSocket() {
//     address.sin_family = AF_INET;
//     address.sin_port = htons(port);

//     if (inet_pton(AF_INET, hostname.c_str(), &address.sin_addr) <= 0) {
//         std::cerr << "Error: Invalid address/ Address not supported." << std::endl;
//         return false;
//     }

//     if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
//         std::cerr << "Error: Failed to bind socket. Errno: " << strerror(errno) << std::endl;
//         return false;
//     }

//     return true;
// }

// Example: Server class method for binding
bool Server::bindSocket() {
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = inet_addr(hostname.c_str()); // Use the hostname from config
    address.sin_port = htons(port); // Set the port

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return false;
    }
    return true;
}



bool Server::makeNonBlocking() {
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error: Failed to get socket flags." << std::endl;
        return false;
    }

    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Error: Failed to make socket non-blocking." << std::endl;
        return false;
    }

    return true;
}

void Server::start() {
    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Error: Failed to listen on socket." << std::endl;
        return;
    }

    std::cout << "Server listening on " << hostname << ":" << port << std::endl;

    // Accept client connections in a loop
    acceptConnections();
}

void Server::acceptConnections() {
    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addr_len);

        if (client_fd < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No incoming connection, non-blocking mode
                continue;
            } else {
                std::cerr << "Error: Failed to accept connection. Errno: " << strerror(errno) << std::endl;
                break;
            }
        }

        // Handle client request
        handleClient(client_fd);

        // Close client connection
        close(client_fd);
    }
}

#include <poll.h>

void Server::handleClient(int client_fd) {
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;  // Wait for data to be ready for reading

    int poll_res = poll(&pfd, 1, 3000);  // Timeout of 3 seconds
    if (poll_res < 0) {
        std::cerr << "Error: Polling failed." << std::endl;
        return;
    } else if (poll_res == 0) {
        std::cerr << "Timeout: No data received from client." << std::endl;
        return;
    }

    if (pfd.revents & POLLIN) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read < 0) {
            std::cerr << "Error: Failed to read from client." << std::endl;
            return;
        }

        std::cout << "Received request: " << buffer << std::endl;

        // Send a simple HTTP response
        std::string http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";

        ssize_t bytes_sent = send(client_fd, http_response.c_str(), http_response.size(), 0);
        if (bytes_sent < 0) {
            std::cerr << "Error: Failed to send response to client." << std::endl;
        }
    }
}

std::string Server::getFilePath(const std::string& request_path) {
    std::string base_directory = "./www";  // Define the base directory for static files
    std::string file_path = base_directory + request_path;

    // If request is for a directory, append index.html or another default file
    if (file_path.back() == '/') {
        file_path += "index.html"; // Default file
    }

    return file_path;
}

void Server::serveFile(int client_fd, const std::string& file_path) {
    // Open the file
    std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        send404(client_fd);  // File not found, send a 404 response
        return;
    }

    // Read file contents into a string
    std::string file_content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    file.close();

    // Determine the content type based on file extension
    std::string content_type = getContentType(file_path);

    // Send HTTP response headers
    std::string http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + content_type + "\r\n"
        "Content-Length: " + std::to_string(file_content.size()) + "\r\n"
        "\r\n";
    
    // Send the headers and the file content
    send(client_fd, http_response.c_str(), http_response.size(), 0);
    send(client_fd, file_content.c_str(), file_content.size(), 0);
}

std::string Server::getContentType(const std::string& file_path) {
    // Check the file extension manually instead of using ends_with
    if (file_path.substr(file_path.size() - 5) == ".html") return "text/html";
    if (file_path.substr(file_path.size() - 4) == ".css") return "text/css";
    if (file_path.substr(file_path.size() - 3) == ".js") return "application/javascript";
    if (file_path.substr(file_path.size() - 4) == ".jpg" || file_path.substr(file_path.size() - 5) == ".jpeg") return "image/jpeg";
    if (file_path.substr(file_path.size() - 4) == ".png") return "image/png";
    if (file_path.substr(file_path.size() - 4) == ".gif") return "image/gif";

    return "application/octet-stream"; // Default content type
}


void Server::send404(int client_fd) {
    std::string http_response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "404 Not Found";
    
    send(client_fd, http_response.c_str(), http_response.size(), 0);
}


