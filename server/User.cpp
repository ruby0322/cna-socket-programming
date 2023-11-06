#include "User.hpp"

User::User() {
    
}

User::User(std::string username) : username{username} {

}

User::~User() {

}

void User::withdraw(int amount) {
    balance -= amount;
}

void User::deposit(int amount) {
    balance += amount;
}