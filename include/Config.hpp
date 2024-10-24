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

// include/Config.hpp
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

struct ServerConfig {
    int port;
    std::string hostname;
    std::string root;
    std::string index;
};

class Config {
public:
    Config(const std::string& filePath);
    ~Config();

    void parse();
    const std::vector<ServerConfig>& getServers() const;

private:
    std::string filePath;
    std::vector<ServerConfig> servers;

    void parseServerBlock();
    void trim(std::string& str);
    int extractPort(const std::string& line);
    std::string extractServerName(const std::string& line);
    std::string extractRoot(const std::string& line);
    std::string extractIndex(const std::string& line);
};

#endif
