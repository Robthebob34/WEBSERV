// include/Config.hpp
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>

class Config {
public:
    Config();
    bool parseConfigFile(const std::string& filename);
    
    // Getters for port and hostname
    int getPort() const;
    std::string getHostname() const;

private:
    int port;
    std::string hostname;

    // Helper functions
    void parseLine(const std::string& line);
};

#endif
