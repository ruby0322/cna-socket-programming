#define LOGGER_HPP

#include <string>
#include <vector>

class Logger {
protected:
    std::string prefix;
    char mode;
    template<typename ...Args> void log(Args&... args) const;
    void wrap(std::string message) const;
    Logger();
    ~Logger();
public:
    void setMode(char mode, std::string prefix);
};

class ClientLogger : public Logger {
public:
    ClientLogger();
    ~ClientLogger();
    void clientUp(std::string addr) const;
    void info(std::string text) const;
    void message(std::string addr, std::string message) const;
    void peerConnection(std::string addr) const;
    void peerDisconnection(std::string addr) const;
    void cliOptions() const;
    void cliInvalidInput(std::string reason) const;
    void cliPrompt(std::string promt) const;
};

class ServerLogger : public Logger {
public:
    ServerLogger();
    ~ServerLogger();
    void debug(std::string text) const;
    void serverUp(int port) const;
    void serverDown() const;
    void error(std::exception &exception) const;
    void newConnection(int numClient, std::string addr) const;
    void lostConnection(int numClient, std::string addr) const;
    void userRegisterAttempt(std::string addr) const;
    void userRegisterSuccess(std::string addr, std::string username) const;
    void userRegisterFailure(std::string addr) const;
    void userDisconnect(std::string addr, std::string username) const;
    void userListRequest(std::string username) const;
    void unauthedUserRequest(std::string addr) const;
    void userMessage(std::string addr, std::string username, std::string message) const;
    void transferSuccess(std::string fromUser, std::string toUser, int amount) const;
    void transferFailureInsufficientBalance(std::string fromUser, std::string toUser, int amount) const;
    void transferFailureNegativeAmount(std::string fromUser, std::string toUser, int amount) const;
    void transferFailureNoPermission(std::string fromUser, std::string attemptedFromUser, std::string toUser, int amount) const;
    void transferFailureInvalidPayee(std::string fromUser, std::string toUser, int amount) const;
    void userLoginFailure(std::string addr, std::string username) const;
    void userLoginSuccess(std::string addr, std::string username, int port) const;

};