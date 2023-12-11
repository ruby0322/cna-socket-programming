#ifndef SOCKET_SERVER_HPP
  #include "SocketServer.hpp"
#endif
#ifndef USER_HPP
    #include "User.hpp"
#endif
#ifndef UTILS_HPP
    #include "../shared/utils.hpp"
#endif

#include <iostream>

int SocketServer::numClient = 0;
int SocketServer::port = 8000;
ServerLogger SocketServer::logger;
std::map<std::string, User> SocketServer::registeredUsers;
std::map<std::string, ClientInfo> SocketServer::connectedUsers;
std::string SocketServer::publicKey;
std::string SocketServer::privateKey;

bool SocketServer::isClientRegistered(std::string addr) {
    return connectedUsers[addr].ip.size() > 0;
}

bool SocketServer::isUsernameTaken(std::string username) {
    return registeredUsers.find(username) != registeredUsers.end();
}

SocketServer::SocketServer(char mode, std::string prefix) {
    logger.setMode(mode, prefix);

    // Generate RSA key pair for the server
    RSA* rsa = RSA_generate_key(BUFFER_SIZE, RSA_F4, nullptr, nullptr);
    if (!rsa) {
        throw std::runtime_error("Error generating RSA key pair.");
    }
    publicKey = getRSAPublicKeyString(rsa);
    privateKey = getRSAPrivateKeyString(rsa);
    RSA_free(rsa);
}

SocketServer::~SocketServer() {}

void SocketServer::setPort(int port) {
    SocketServer::port = port;
}

std::string SocketServer::getOnlineList(std::string addr) {
    std::string list, username;
    if (connectedUsers[addr].ip.size() > 0) {
        logger.userListRequest(connectedUsers[addr].ip);
        list += std::to_string(registeredUsers[connectedUsers[addr].ip].getBalance());
        list += '\n';
    } else {
        logger.unauthedUserRequest(addr);
        return Code::Auth::AUTH_FAIL;
    }
    list += "<SERVER_PUBLIC_KEY>";
    list += '\n';
    list += std::to_string(connectedUsers.size());
    list += '\n';
    for (auto& [addr, p] : connectedUsers) {
        list += p.ip;
        list += '#';
        list += addr.substr(0, addr.find(':'));
        list += '#';
        list += std::to_string(p.port);
        list += '\n';
    }
    return list;
}

void SocketServer::setup() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SocketServer::port); 
    // serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    bind(socketFd, (sockaddr*)&serverAddress, sizeof(serverAddress));
}

void SocketServer::newConnection(std::string addr) {
    logger.newConnection(++numClient, addr);
}

void SocketServer::lostConnection(std::string addr) {
    logger.lostConnection(--numClient, addr);
}

void* SocketServer::handleClient(void* arg) {
    int clientSocket = *((int*)arg);
    std::string addr = getAddr(clientSocket).toString();
    newConnection(addr);
    connectedUsers[addr];

    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);

    if (bytesRead <= 0) {
        return nullptr;
    }

    send(clientSocket, publicKey.c_str(), strlen(publicKey.c_str()), 0);

    connectedUsers[addr].publicKey = std::string(buffer, buffer+bytesRead);

    SocketServer::clientHandler(clientSocket);
    close(clientSocket);
    lostConnection(addr);   
    connectedUsers.erase(addr);
    return nullptr;
}

void SocketServer::listen() {
    setup();
    int newSocket;
    sockaddr_in clientAddress;
    ::listen(socketFd, MAX_CLIENTS);
    logger.serverUp(SocketServer::port);
    while (true) {
        int clientSize = sizeof(clientAddress);
        newSocket = accept(socketFd, (sockaddr*)&clientAddress, (socklen_t*)&clientSize);
        pthread_t threadId;
        pthread_create(&threadId, nullptr, handleClient, &newSocket);
    }
    logger.serverDown();
}

std::string SocketServer::registerUser(std::string username, std::string addr) {
    logger.userRegisterAttempt(addr); 
    if (isClientRegistered(addr) || isUsernameTaken(username)) {
        logger.userRegisterFailure(addr);
        return Code::FAIL;
    } else if (username.find('#') != std::string::npos) {
        logger.userRegisterFailure(addr);
        return Code::Auth::REGISTER_FAIL_INVALID_CHAR;
    } else {
        registeredUsers[username] = {username};
        logger.userRegisterSuccess(addr, username);
        return Code::OK;
    }
}

std::string SocketServer::loginUser(std::string addr, std::string username, int port) {
    if (isUsernameTaken(username)) {
        logger.userLoginSuccess(addr, username, port);
        connectedUsers[addr].ip = username;
        connectedUsers[addr].port = port;
    } else {
        logger.userLoginFailure(addr, username);
    }
    return getOnlineList(addr);
}

std::string SocketServer::handleTransferRequest(std::string addr, std::string req) {
    std::string res;
    if (isClientRegistered(addr)) {
        size_t firstOccurence = req.find('#');
        size_t secondOccurence = req.find('#', firstOccurence+1);
        int amount = std::stoi(req.substr(firstOccurence+1, secondOccurence));
        std::string fromUser = req.substr(0, firstOccurence);
        std::string toUser = req.substr(secondOccurence+1);

        if (toUser != connectedUsers[addr].ip) {
            res = Code::Transfer::FAIL_NO_PERMISSION;
            logger.transferFailureNoPermission(connectedUsers[addr].ip, fromUser, toUser, amount);
        } else if (!isUsernameTaken(toUser) || !isUsernameTaken(fromUser)) {
            res = Code::Transfer::FAIL_INVALID_USER;
            logger.transferFailureInvalidPayee(fromUser, toUser, amount);
        } else if (amount < 0) {
            res = Code::Transfer::FAIL_NEGATIVE_AMOUNT;
            logger.transferFailureNegativeAmount(fromUser, toUser, amount);
        } else {
            res = transfer(fromUser, toUser, amount);
        }
    } else {
        res = Code::Auth::AUTH_FAIL;
        logger.unauthedUserRequest(addr);
    }
    return res;
}

std::string SocketServer::handleRequest(int clientSocket, std::string req) {
    std::string addr = getAddr(clientSocket).toString();
    if (isClientRegistered(addr)) {
        logger.userMessage(addr, connectedUsers[addr].ip, req);
    } else {
        logger.userMessage(addr, "", req);
    }

    std::string res;
    try {
        if (req == "Exit") {
            res = "Bye\n";
            logger.userDisconnect(addr, connectedUsers[addr].ip);
        } else if (req == "List") {
            res = getOnlineList(addr);
        } else if (req.size() >= 9 && req.substr(0, 9) == "REGISTER#") {
            res = registerUser(req.substr(9), addr);
        } else if (std::count(req.begin(), req.end(), '#') == 1 && isNumber(req.substr(req.find('#')+1))) {
            std::string username = req.substr(0, req.find('#'));
            int port = std::stoi(req.substr(req.find('#')+1));
            res = loginUser(addr, username, port);
        } else if (std::count(req.begin(), req.end(), '#') == 2) {
            res = handleTransferRequest(addr, req);
        } else {
            res = Code::BAD_REQUEST;
        }
    } catch (std::exception &e) {
        logger.error(e);
        res = Code::ERROR;
    }

    return res;
}

void SocketServer::clientHandler(int clientSocket) {
  while (true) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesRead <= 0) {
        break;
    }
    std::string rawMessage(buffer, buffer+bytesRead);
    logger.rawMessage(rawMessage);
    std::string req = decryptMessage(rawMessage, privateKey);
    std::string res = handleRequest(clientSocket, req);

    sendEncryptedMessage(clientSocket, res, connectedUsers[getAddr(clientSocket).toString()].publicKey);
    if (req == "Exit") break;
  }
}

std::string SocketServer::transfer(std::string fromUser, std::string toUser, int amount) {
    try {
        if (registeredUsers[fromUser].getBalance() >= amount) {
            registeredUsers[fromUser].withdraw(amount);
            registeredUsers[toUser].deposit(amount);
            logger.transferSuccess(fromUser, toUser, amount);
            return Code::OK;
        } else {
            logger.transferFailureInsufficientBalance(fromUser, toUser, amount);
            return Code::Transfer::FAIL_INSUFFICIENT_BALANCE;
        }
    } catch (std::exception &e) {
        logger.error(e);
        return Code::FAIL;
    }
}