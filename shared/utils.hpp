#define UTILS_HPP

#include <iostream>
#include <stdio.h>  
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <tuple>
#include <vector>

#ifndef ADDRESS_HPP
    #include "Address.hpp"
#endif


bool isNumber(std::string s);
Address getAddr(int clientSocket);
std::tuple<std::string, int, std::string> parseTransferRequest(std::string req);
std::tuple<std::string, std::string, int> parseOnlineListLine(std::string line);
std::vector<std::tuple<std::string, std::string, int>> parseOnlineList(std::string list);
bool validateUsername(std::string username);