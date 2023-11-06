#define USER_HPP

#include <string>

class User {
private:
    std::string username;
    std::string ip;
    int port;
    int balance = 1000;
public:
    User(std::string username);
    User();
    ~User();
    std::string getUsername() const { return username; }
    int getBalance() const { return balance; }
    void withdraw(int amount);
    void deposit(int amount);
};