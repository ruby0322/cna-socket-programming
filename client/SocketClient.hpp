#define SOCKET_CLIENT_HPP

#include <iostream>
#include <stdio.h>  
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <map>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

#ifndef ADDRESS_HPP
  #include "../shared/Address.hpp"
#endif
#ifndef UTILS_HPP
  #include "../shared/utils.hpp"
#endif
#ifndef LOGGER_HPP
  #include "../shared/Logger.hpp"
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
  static std::string serverIp;
  static int serverPort;

  static bool up;
  static int listeningPeerSocketFd;
  static int connectingPeerSocketFd;
  static int serverSocketFd;
  static std::map<std::string, Address> peerAddresses;  // username -> Address

  static ClientLogger logger;
  static std::string serverPublicKey;
  static std::string publicKey;
  static std::string privateKey;
  static std::string peerPublicKey;

public:
  SocketClient(std::string prefix);
  ~SocketClient();
  void run();
  static void* listen(void* arg);
  static void* handleCommand(void* arg);
  static void updatePeerPorts(std::string list);
  static void* onServerConnect(void* arg);
  static void sendMessageToPeer(Address addr, const std::string& message);
  static void* onPeerConnect(void* arg);
  static bool onCommand(std::string cmd);
  static void setServer(std::string ip, int port);
  static std::string commandLineInterface();
  static void generateRSAKeyPair();  
};