// src/Server.cpp
#include "../include/Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

Server::Server(const std::string& hostname, int port)
    : hostname(hostname), port(port), server_fd(-1) {}

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

bool Server::bindSocket() {
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, hostname.c_str(), &address.sin_addr) <= 0) {
        std::cerr << "Error: Invalid address/ Address not supported." << std::endl;
        return false;
    }

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Error: Failed to bind socket." << std::endl;
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

void Server::acceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd < 0) {
        std::cerr << "Error: Failed to accept client connection." << std::endl;
        return;
    }

    // Set the client socket to non-blocking mode
    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    // Add the client socket to pollfd for monitoring
    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;  // We want to monitor for incoming data (POLLIN)
    fds.push_back(pfd);

    std::cout << "Accepted connection from client." << std::endl;
}


void Server::handleClient(int client_fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // Receive client request
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Non-blocking mode: no data available to read
            std::cerr << "No data available to read right now (non-blocking mode)." << std::endl;
            return;
        } else {
            std::cerr << "Error: Failed to read from client (errno: " << strerror(errno) << ")." << std::endl;
            return;
        }
    }

    if (bytes_read == 0) {
        std::cerr << "Client closed the connection." << std::endl;
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
        std::cerr << "Error: Failed to send response to client (errno: " << strerror(errno) << ")." << std::endl;
    }
}

