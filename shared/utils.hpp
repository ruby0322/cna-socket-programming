#define UTILS_HPP

#include <iostream>
#include <stdio.h>  
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <tuple>
#include <vector>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#ifndef ADDRESS_HPP
    #include "Address.hpp"
#endif

#define __debug(x) std::cout << #x << ": { " << (x) << " }" << std::endl
#define __flag(x) std::cout << "|>" << std::endl << "|    FLAG " << (x) << std::endl << "|" << std::endl
#define BUFFER_SIZE 2048


bool isNumber(const std::string& s);
Address getAddr(int clientSocket);
std::tuple<std::string, int, std::string> parseTransferRequest(const std::string& req);
std::tuple<std::string, std::string, int> parseOnlineListLine(const std::string& line);
std::vector<std::tuple<std::string, std::string, int>> parseOnlineList(std::string list);
bool validateUsername(const std::string& username);
std::string getRSAPublicKeyString(RSA* rsa);
std::string getRSAPrivateKeyString(RSA* rsa);
RSA* getRSAPublicKey(const std::string& keyString);
RSA* getRSAPrivateKey(const std::string& keyString);
std::string encryptMessage(const std::string& plaintext, const std::string& publicKey);
std::string decryptMessage(const std::string& encryptedMessage, const std::string& privateKey);
void sendEncryptedMessage(int socketFd, const std::string& message, const std::string& publicKey);
void printSSLError();