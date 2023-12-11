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

void ClientLogger::clientUp(const std::string& addr) const {
    log("Client up on ", addr, ".");
}

void ClientLogger::info(const std::string& text) const {
    log("[Info]");
    wrap(text);
}

void Logger::message(const std::string& addr, const std::string& message) const {
    log("[Decrypted Message] ", addr);
    wrap(message);
}

void ClientLogger::peerConnection(const std::string& addr) const {
    log("[Connection] New");
    wrap("Peer " + addr + " connected");
}

void ClientLogger::peerDisconnection(const std::string& addr) const {
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

void ClientLogger::cliInvalidInput(const std::string& reason) const {
    log("");
    log("[CLI] Invalid input. Try again.");
    wrap("Reason: " + reason);
}

void ClientLogger::cliPrompt(const std::string& prompt) const {
    log("");
    log("[CLI] ", prompt);
}

ServerLogger::ServerLogger() {}

ServerLogger::~ServerLogger() {}

void Logger::setMode(char mode, const std::string& prefix) {
    this->mode = mode;
    this->prefix = prefix;
}

void ServerLogger::debug(const std::string& text) const {
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

void ServerLogger::newConnection(int numClient, const std::string& addr) const {
    log("");
    log("[Connection] New: ",  addr);
    log("There are currently ", numClient, " client(s) connected.");
}

void ServerLogger::lostConnection(int numClient, const std::string& addr) const {
    log("");
    log("[Connection] Lost: ", addr);
    log("There are currently ", numClient, " client(s) connected.");
}

void ServerLogger::userRegisterAttempt(const std::string& addr) const {
    log("");
    log("Client ", addr, " attempted to register.");
}

void ServerLogger::userRegisterSuccess(const std::string& addr, const std::string& username) const {
    log("");
    log("Client ", addr, " successfully registered as ", username, ".");
}

void ServerLogger::userRegisterFailure(const std::string& addr) const {
    log("");
    log("Client ", addr, " failed to register.");
}

void ServerLogger::userDisconnect(const std::string& addr, const std::string& username) const {
    log("");
    if (username.size() > 0) {
        log("Client ", addr, ", registered as ", username, ", disconnected.");
    } else {
        log("Client ", addr, " disconnected.");
    }
}

void ServerLogger::userListRequest(const std::string& username) const {
    log("");
    log("Registered user ", username, " requested online list.");
}

void ServerLogger::unauthedUserRequest(const std::string& addr) const {
    log("");
    log("An unauthed client ", addr, " attempted to request restricted resources.");
}

void ServerLogger::userMessage(const std::string& addr, const std::string& username, const std::string& message) const {
    if (username.size() > 0) {
        log("[Decrypted Message] ", addr, " (", username, ")");
    } else {
        log("[Decrypted Message] ", addr, " ([unauthed])");
    }
    wrap(message);
}

void ServerLogger::transferSuccess(const std::string& fromUser, const std::string& toUser, int amount) const {
    log("[Transfer] Success");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --> " + toUser);
}

void ServerLogger::transferFailureInsufficientBalance(const std::string& fromUser, const std::string& toUser, int amount) const {
    log("[Transfer] Failure due to insufficient balance");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --x " + toUser);
}

void ServerLogger::transferFailureNegativeAmount(const std::string& fromUser, const std::string& toUser, int amount) const {
    log("[Transfer] Failure due to negative amount");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --x " + toUser);
}

void ServerLogger::transferFailureNoPermission(const std::string& fromUser, const std::string& attemptedFromUser, const std::string& toUser, int amount) const {
    log("[Transfer] Failure due to no permission");
    wrap(fromUser + ": " + attemptedFromUser + " -- $" + std::to_string(amount) + " --x " + toUser);
}

void ServerLogger::transferFailureInvalidPayee(const std::string& fromUser, const std::string& toUser, int amount) const {
    log("[Transfer] Failure due to no permission");
    wrap(fromUser + " -- $" + std::to_string(amount) + " --x " + toUser + " ([invalid])");
}

void ServerLogger::userLoginFailure(const std::string& addr, const std::string& username) const {
    log("[Login] Failure");
    wrap(addr + " <=x=> " + username);
}

void ServerLogger::userLoginSuccess(const std::string& addr, const std::string& username, int port) const {
    log("[Login] Success");
    wrap(addr + " <===> " + username);
}

void Logger::rawMessage(const std::string& message) const {
    log("[Raw Message]");
    log(message);
}