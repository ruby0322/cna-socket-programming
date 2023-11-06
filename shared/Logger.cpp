#ifndef LOGGER_HPP
    #include "Logger.hpp"
#endif

#include <iostream>

Logger::Logger() {}

Logger::~Logger() {}

template<typename ...Args> void Logger::log(Args&... args) const {
    std::cout << '[' << prefix << "] ";
    (std::cout << ... << args);
    std::cout << std::endl;
}

void Logger::wrap(std::string message) const {
    log("┌--------------------------------------------------┐");
    if (message.back() == '\n' && message.size() > 0) 
        message = message.substr(0, message.size()-1);
    size_t prev = 0;
    size_t curr = message.find('\n');
    while (prev != std::string::npos) {
        std::string line;
        if (prev == 0) {
            line = message.substr(0, curr);
        } else if (curr == std::string::npos) {
            line = message.substr(prev+1);
        } else  {
            line = message.substr(prev+1, curr-prev-1);
        }
        log("| ", line);
        prev = curr;
        if (curr != std::string::npos) 
            curr = message.find('\n', curr+1);
    }
    log("└--------------------------------------------------┘");
}

ClientLogger::ClientLogger() {}

ClientLogger::~ClientLogger() {}

void ClientLogger::clientUp(std::string addr) const {
    log("Client up on ", addr, ".");
}

void ClientLogger::info(std::string text) const {
    log("[Info]");
    wrap(text);
}

void ClientLogger::message(std::string addr, std::string message) const {
    log("[Message] ", addr);
    wrap(message);
}

void ClientLogger::peerConnection(std::string addr) const {
    log("[Connection] New");
    wrap("Peer " + addr + " connected");
}

void ClientLogger::peerDisconnection(std::string addr) const {
    log("[Connection] Lost");
    wrap("Peer " + addr + " disconnected");
}

void ClientLogger::cliOptions() const {
    std::string options; 
    options += "(a) Login\n";
    options += "(b) Register\n";
    options += "(c) Status List\n";
    options += "(d) Transfer\n";
    options += "(e) Exit\n";
    log("[CLI] Enter option to interact");
    wrap(options);
}

void ClientLogger::cliInvalidInput(std::string reason) const {
    log("");
    log("[CLI] Invalid input. Try again.");
    wrap("Reason: " + reason);
}

void ClientLogger::cliPrompt(std::string prompt) const {
    log("");
    log("[CLI] ", prompt);
}

ServerLogger::ServerLogger() {}

ServerLogger::~ServerLogger() {}

void Logger::setMode(char mode, std::string prefix) {
    this->mode = mode;
    this->prefix = prefix;
}

void ServerLogger::debug(std::string text) const {
    log("Debug: ", text);
}

void ServerLogger::error(std::exception &exception) const {
    std::string error = exception.what();
    log("[Error]");
    wrap(error);
}

void ServerLogger::serverUp(int port) const {
    log("Socket server is now up on port ", port, ".");
}

void ServerLogger::serverDown() const {
    log("");
    log("Socket server is now down.");
}

void ServerLogger::newConnection(int numClient, std::string addr) const {
    log("");
    log("[Connection] New: ",  addr);
    log("There are currently ", numClient, " client(s) connected.");
}

void ServerLogger::lostConnection(int numClient, std::string addr) const {
    log("");
    log("[Connection] Lost: ", addr);
    log("There are currently ", numClient, " client(s) connected.");
}

void ServerLogger::userRegisterAttempt(std::string addr) const {
    log("");
    log("Client ", addr, " attempted to register.");
}

void ServerLogger::userRegisterSuccess(std::string addr, std::string username) const {
    log("");
    log("Client ", addr, " successfully registered as ", username, ".");
}

void ServerLogger::userRegisterFailure(std::string addr) const {
    log("");
    log("Client ", addr, " failed to register.");
}

void ServerLogger::userDisconnect(std::string addr, std::string username) const {
    log("");
    if (username.size() > 0) {
        log("Client ", addr, ", registered as ", username, ", disconnected.");
    } else {
        log("Client ", addr, " disconnected.");
    }
}

void ServerLogger::userListRequest(std::string username) const {
    log("");
    log("Registered user ", username, " requested online list.");
}

void ServerLogger::unauthedUserRequest(std::string addr) const {
    log("");
    log("An unauthed client ", addr, " attempted to request restricted resources.");
}

void ServerLogger::userMessage(std::string addr, std::string username, std::string message) const {
    if (username.size() > 0) {
        log("[Message] ", addr, " (", username, ")");
    } else {
        log("[Message] ", addr, " ([unauthed])");
    }
    wrap(message);
}

void ServerLogger::transferSuccess(std::string fromUser, std::string toUser, int amount) const {
    log("[Transfer] Success");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --> " + toUser);
}

void ServerLogger::transferFailureInsufficientBalance(std::string fromUser, std::string toUser, int amount) const {
    log("[Transfer] Failure due to insufficient balance");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --x " + toUser);
}

void ServerLogger::transferFailureNegativeAmount(std::string fromUser, std::string toUser, int amount) const {
    log("[Transfer] Failure due to negative amount");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --x " + toUser);
}

void ServerLogger::transferFailureNoPermission(std::string fromUser, std::string attemptedFromUser, std::string toUser, int amount) const {
    log("[Transfer] Failure due to no permission");
    wrap(fromUser + ": " + attemptedFromUser + " -- $" + std::to_string(amount) + " --x " + toUser);
}

void ServerLogger::transferFailureInvalidPayee(std::string fromUser, std::string toUser, int amount) const {
    log("[Transfer] Failure due to no permission");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --x " + toUser + " ([invalid])");
}

void ServerLogger::userLoginFailure(std::string addr, std::string username) const {
    log("[Login] Failure");
    wrap(addr + " <=x=> " + username);
}

void ServerLogger::userLoginSuccess(std::string addr, std::string username, int port) const {
    log("[Login] Success");
    wrap(addr + " <===> " + username);
}