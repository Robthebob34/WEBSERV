// src/main.cpp
#include <iostream>
#include "../include/Config.hpp"
#include "../include/Server.hpp"

int main(int argc, char** argv) {
    Config config;

    // Parse configuration file if provided
    if (argc > 1) {
        if (!config.parseConfigFile(argv[1])) {
            std::cerr << "Failed to parse configuration file." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "No configuration file provided. Using default values." << std::endl;
    }

    // Create and set up the server
    Server server(config.getHostname(), config.getPort());
    if (!server.setup()) {
        std::cerr << "Error: Failed to set up the server." << std::endl;
        return 1;
    }

    // Start the server
    server.start();

    return 0;
}
