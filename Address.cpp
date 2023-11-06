#include <iostream>
#include <stdio.h>  
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "Address.hpp"

std::string Address::toString() const { 
    return ip + ':' + std::to_string(port);
}