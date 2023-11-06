#define SOCKET_SERVER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <map>
#include <set>
#include "../shared/Logger.hpp"
#include "User.hpp"

namespace Code {
  const std::string BAD_REQUEST = "200 BAD_REQUEST\n";
  const std::string OK = "100 OK\n";
  const std::string FAIL = "210 FAIL\n";
  const std::string ERROR = "220 ERROR\n";
  namespace Auth {
    const std::string AUTH_FAIL = "220 AUTH_FAIL\n";
    const std::string REGISTER_FAIL_INVALID_CHAR = "221 REGISTER FAIL DUE TO INVALID CHARACTER\n";
  };
  namespace Transfer {
    const std::string FAIL_INSUFFICIENT_BALANCE = "211 TRANSFER FAIL DUE TO INSUFFICIENT BALANCE\n";
    const std::string FAIL_NEGATIVE_AMOUNT = "212 TRANSFER FAIL DUE TO NEGATIVE AMOUNT\n";
    const std::string FAIL_NO_PERMISSION = "213 TRANSFER FAIL DUE TO NO PERMISSION\n";
    const std::string FAIL_INVALID_USER = "214 TRANSFER FAIL DUE TO INVALID USER\n";
  };
};

class SocketServer {
private:

  static const int MAX_CLIENTS = 100;
  static const int BUFFER_SIZE = 1024;
  static const int PORT = 8000;
  
  char mode;
  int socketFd;
  sockaddr_in serverAddress;
  static ServerLogger logger;
  static int numClient;
  static std::map<std::string, std::pair<std::string, int>> connectedUsers;
  static std::map<std::string, User> registeredUsers;
public:
  SocketServer(char mode, std::string prefix);
  ~SocketServer();
  void setup();
  void listen();
  static bool isClientRegistered(std::string addr);
  static bool isUsernameTaken(std::string username);
  static std::string handleRequest(int clientSocket, std::string request);
  static void* handleClient(void* arg);
  static std::string getOnlineList(std::string addr);
  static std::string registerUser(std::string username, std::string addr);
  static std::string loginUser(std::string addr, std::string username, int port);
  static void clientHandler(int socket);
  static void newConnection(std::string addr);
  static void lostConnection(std::string addr);
  static std::string handleTransferRequest(std::string addr, std::string req);
  static std::string transfer(std::string fromUser, std::string toUser, int amount);
};