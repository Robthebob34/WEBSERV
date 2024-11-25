#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//server.hpp
#include <string>
#include <netinet/in.h>
#include <poll.h>
#include <cstdlib>


//config.hpp
#include <string>
#include <vector>
#include <fstream>

//cgi.hpp
#include <iostream>
#include <map>
#include <algorithm>
#include <unistd.h>


//Msg.hpp
#include <sys/stat.h>
#include <fcntl.h>

//Main.cpp
#include <csignal>
#include "Cgi.hpp"
#include "Config.hpp"
#include "Msg.hpp"
#include "Server.hpp"
#include "Request.hpp"

//utils.cpp
#include <iostream>
#include <sstream> // Add this header
#include <string>  // If you are using std::string
#include <iostream> // If you are using std::cout or similar features

std::string myItoa(int to_translate);


#endif
