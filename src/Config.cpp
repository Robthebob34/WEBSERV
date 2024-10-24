#include "../include/Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

Config::Config() : port(8080), hostname("localhost") {
    // Default values (in case config file doesn't specify)
}

bool Config::parseConfigFile(const std::string& filename) {
    std::ifstream configFile(filename.c_str());
    if (!configFile.is_open()) {
        std::cerr << "Error: Unable to open configuration file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        parseLine(line);
    }

    configFile.close();
    return true;
}

void Config::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string key;
    
    // Split line into key-value pairs
    if (std::getline(iss, key, '=')) {
        std::string value;
        if (std::getline(iss, value)) {
            if (key == "port") {
                port = std::stoi(value);  // Convert port from string to int
            } else if (key == "hostname") {
                hostname = value;
            }
        }
    }
}

int Config::getPort() const {
    return port;
}

std::string Config::getHostname() const {
    return hostname;
}
