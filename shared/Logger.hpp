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
    void setMode(char mode, const std::string& prefix);
    void rawMessage(const std::string& message) const;
    void message(const std::string& addr, const std::string& message) const;
};

class ClientLogger : public Logger {
public:
    ClientLogger();
    ~ClientLogger();
    void clientUp(const std::string& addr) const;
    void info(const std::string& text) const;
    void peerConnection(const std::string& addr) const;
    void peerDisconnection(const std::string& addr) const;
    void cliOptions() const;
    void cliInvalidInput(const std::string& reason) const;
    void cliPrompt(const std::string& promt) const;
};

class ServerLogger : public Logger {
public:
    ServerLogger();
    ~ServerLogger();
    void debug(const std::string& text) const;
    void serverUp(int port) const;
    void serverDown() const;
    void error(std::exception &exception) const;
    void newConnection(int numClient, const std::string& addr) const;
    void lostConnection(int numClient, const std::string& addr) const;
    void userRegisterAttempt(const std::string& addr) const;
    void userRegisterSuccess(const std::string& addr, const std::string& username) const;
    void userRegisterFailure(const std::string& addr) const;
    void userDisconnect(const std::string& addr, const std::string& username) const;
    void userListRequest(const std::string& username) const;
    void unauthedUserRequest(const std::string& addr) const;
    void userMessage(const std::string& addr, const std::string& username, const std::string& message) const;
    void transferSuccess(const std::string& fromUser, const std::string& toUser, int amount) const;
    void transferFailureInsufficientBalance(const std::string& fromUser, const std::string& toUser, int amount) const;
    void transferFailureNegativeAmount(const std::string& fromUser, const std::string& toUser, int amount) const;
    void transferFailureNoPermission(const std::string& fromUser, const std::string& attemptedFromUser, const std::string& toUser, int amount) const;
    void transferFailureInvalidPayee(const std::string& fromUser, const std::string& toUser, int amount) const;
    void userLoginFailure(const std::string& addr, const std::string& username) const;
    void userLoginSuccess(const std::string& addr, const std::string& username, int port) const;
};