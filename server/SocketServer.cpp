#ifndef SOCKET_SERVER_HPP
  #include "SocketServer.hpp"
#endif
#ifndef USER_HPP
    #include "User.hpp"
#endif
#ifndef UTILS_HPP
    #include "../utils.hpp"
#endif

#include <iostream>

int SocketServer::numClient = 0;
ServerLogger SocketServer::logger;
std::map<std::string, User> SocketServer::registeredUsers;
std::map<std::string, std::pair<std::string, int>> SocketServer::connectedUsers;

bool SocketServer::isClientRegistered(std::string addr) {
    return connectedUsers[addr].first.size() > 0;
}

bool SocketServer::isUsernameTaken(std::string username) {
    return registeredUsers.find(username) != registeredUsers.end();
}

SocketServer::SocketServer(char mode, std::string prefix) {
    setup();
    logger.setMode(mode, prefix);
}

SocketServer::~SocketServer() {
    ;
}

std::string SocketServer::getOnlineList(std::string addr) {
    std::string list, username;
    if (connectedUsers[addr].first.size() > 0) {
        logger.userListRequest(connectedUsers[addr].first);
        list += std::to_string(registeredUsers[connectedUsers[addr].first].getBalance());
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
        list += p.first;
        list += '#';
        list += addr.substr(0, addr.find(':'));
        list += '#';
        list += std::to_string(p.second);
        list += '\n';
    }
    return list;
}

void SocketServer::setup() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SocketServer::PORT); 
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
    SocketServer::clientHandler(clientSocket);
    close(clientSocket);
    lostConnection(addr);   
    connectedUsers.erase(addr);
    return nullptr;
}

void SocketServer::listen() {
    int newSocket;
    sockaddr_in clientAddress;
    ::listen(socketFd, MAX_CLIENTS);
    logger.serverUp(SocketServer::PORT);
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
        // connectedUsers[addr] = username;
        registeredUsers[username] = {username};
        logger.userRegisterSuccess(addr, username);
        return Code::OK;
    }
}

std::string SocketServer::loginUser(std::string addr, std::string username, int port) {
    if (isUsernameTaken(username)) {
        logger.userLoginSuccess(addr, username, port);
        connectedUsers[addr].first = username;
        connectedUsers[addr].second = port;
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

        if (toUser != connectedUsers[addr].first) {
            res = Code::Transfer::FAIL_NO_PERMISSION;
            logger.transferFailureNoPermission(connectedUsers[addr].first, fromUser, toUser, amount);
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
        logger.userMessage(addr, connectedUsers[addr].first, req);
    } else {
        logger.userMessage(addr, "", req);
    }

    std::string res;
    try {
        if (req == "Exit") {
            res = "Bye\n";
            logger.userDisconnect(addr, connectedUsers[addr].first);
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
    char buffer[1024] = {0};
    if (recv(clientSocket, buffer, 1024, 0) == 0) {
        break;
    }
    std::string req(buffer), res;
    res = handleRequest(clientSocket, req);
    int cstrlen = strlen(res.c_str());
    send(clientSocket, res.c_str(), cstrlen, 0);
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