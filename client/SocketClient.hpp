#define SOCKET_CLIENT_HPP

#include <iostream>
#include <stdio.h>  
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <map>
#ifndef ADDRESS_HPP
  #include "../Address.hpp"
#endif
#ifndef UTILS_HPP
  #include "../utils.hpp"
#endif
#ifndef LOGGER_HPP
  #include "../Logger.hpp"
#endif

class SocketClient { 
private:
  static Address threadAddressArg;
  static bool listening;
  static bool peerLock;
  static bool messageLock;
  static bool isConnectedToPeer;
  static sockaddr_in serverAddress;
  static sockaddr_in peerAddress;
  static const int MAX_CLIENTS = 100;
  static const int BUFFER_SIZE = 1024;
  static const std::string SERVER_ADDR;
  static const int SERVER_PORT;

  static bool up;
  static int listeningPeerSocketFd;
  static int connectingPeerSocketFd;
  static int serverSocketFd;
  static std::map<std::string, Address> peerAddresses;  // username -> Address

  static ClientLogger logger;
public:
  SocketClient(std::string prefix);
  ~SocketClient();
  void run();
  static void* listen(void* arg);
  static void* handleCommand(void* arg);
  static void updatePeerPorts(std::string list);
  static void* onServerConnect(void* arg);
  static void connectToPeer(Address addr);
  static void* onPeerConnect(void* arg);
  static bool onCommand(std::string cmd);
  static std::string commandLineInterface();
};