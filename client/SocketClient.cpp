#ifndef SOCKET_CLIENT_HPP
    #include "SocketClient.hpp"
#endif

bool SocketClient::up = true;
std::string SocketClient::serverIp = "127.0.0.1";
int SocketClient::serverPort = 8000;
sockaddr_in SocketClient::serverAddress;
sockaddr_in SocketClient::peerAddress;
int SocketClient::serverSocketFd;
int SocketClient::listeningPeerSocketFd;
int SocketClient::connectingPeerSocketFd;
bool SocketClient::listening = false;
Address SocketClient::threadAddressArg;
ClientLogger SocketClient::logger;
std::map<std::string, Address> SocketClient::peerAddresses;
bool SocketClient::peerLock = false;
bool SocketClient::messageLock = false;
bool SocketClient::isConnectedToPeer = false;

SocketClient::SocketClient(std::string prefix) {
    logger.setMode('a', prefix);
    if ((serverSocketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      throw std::runtime_error("Socket Creation Error");
    }
    if ((listeningPeerSocketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      throw std::runtime_error("Socket Creation Error");
    }
    if ((connectingPeerSocketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      throw std::runtime_error("Socket Creation Error");
    }

    

    peerAddress.sin_family = AF_INET;
    
}
  
SocketClient::~SocketClient() {
    close(serverSocketFd);
    close(listeningPeerSocketFd);
}

void SocketClient::setServer(std::string ip, int port) {
  serverIp = ip;
  serverPort = port;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(serverPort); 
  serverAddress.sin_addr.s_addr = inet_addr(serverIp.c_str());
}

void* SocketClient::listen(void* arg) {
    Address addr = *((Address*) arg);
    peerAddress.sin_port = htons(addr.port);
    peerAddress.sin_addr.s_addr = inet_addr(addr.ip.c_str());
    bind(listeningPeerSocketFd, (sockaddr*)&peerAddress, (socklen_t)sizeof(peerAddress));

    int newSocket;
    ::listen(listeningPeerSocketFd, MAX_CLIENTS);
    while (true) {
        int peerAddressSize = sizeof(peerAddress);
        newSocket = accept(listeningPeerSocketFd, (sockaddr*) &peerAddress, (socklen_t *) &peerAddressSize);
        pthread_t peerThread;
        pthread_create(&peerThread, nullptr, onPeerConnect, &newSocket);
    }
  }

void SocketClient::run() {
    pthread_t serverThread, commandThread;

    if (::connect(serverSocketFd, (sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
      throw std::runtime_error("Connection Error");
    }

    pthread_create(&serverThread, nullptr, onServerConnect, nullptr);
    pthread_create(&commandThread, nullptr, handleCommand, nullptr);
    
    pthread_join(serverThread, nullptr);
}

void* SocketClient::handleCommand(void* arg) {
    while (up) {
        while (messageLock);
        std::string req = commandLineInterface();
        if (req.empty()) 
            continue;
        if (onCommand(req))
            messageLock = true;
    }
    return nullptr;
}

void SocketClient::updatePeerPorts(std::string list) {
    peerAddresses.clear();
    list = list.substr(list.find('\n')+1);
    list = list.substr(list.find('\n')+1);
    list = list.substr(list.find('\n')+1);
    auto parsedLines = parseOnlineList(list);
    for (auto& [username, ip, port] : parsedLines) {
      peerAddresses[username] = { ip, port };
    }
    peerLock = false;
}

void* SocketClient::onServerConnect(void* arg) {
    logger.info("Client connected to server: " + serverIp + ':' + std::to_string(serverPort));
    while (up) {
      char buffer[BUFFER_SIZE] = { 0 };
      recv(serverSocketFd, buffer, BUFFER_SIZE, 0);
      std::string message(buffer);
      logger.message(serverIp + ':' + std::to_string(serverPort), message);
      if (std::count(message.begin(), message.end(), '#') > 0) {
        updatePeerPorts(message);
      }
      messageLock = false;
    }
    return nullptr;
}

void SocketClient::connectToPeer(Address addr) {
    sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(addr.port);
    peerAddr.sin_addr.s_addr = inet_addr(addr.ip.c_str());
    if (::connect(connectingPeerSocketFd, (sockaddr *)&peerAddr, sizeof(peerAddr)) == -1) {
      std::cout << "failed to connect to peer " << addr.toString() << std::endl;
      throw std::runtime_error("Connection Error");
    }
    logger.info("Connected to peer " + addr.toString());
    isConnectedToPeer = true;
}

void* SocketClient::onPeerConnect(void* arg) {
    int peerSocket = *((int*)arg);
    Address addr = getAddr(peerSocket);
    logger.info("New connection from peer " + addr.toString());
    while (up) {
      char buffer[BUFFER_SIZE] = {0};
      if (recv(peerSocket, buffer, BUFFER_SIZE, 0) == 0) {
        logger.peerDisconnection(addr.toString());
        close(peerSocket);
        return nullptr;
      }
      std::string msg(buffer);
      logger.message(addr.toString(), msg);
      logger.info("Forwarding peer message to server...");
      send(serverSocketFd, msg.c_str(), msg.size(), 0);
    }
    return nullptr;
}

bool SocketClient::onCommand(std::string cmd) {
    if (cmd == "Exit") {
      up = false;
    } else if (std::count(cmd.begin(), cmd.end(), '#') == 1 && isNumber(cmd.substr(cmd.find('#')+1))) {
      if (!listening) {
        int port = std::stoi(cmd.substr(cmd.find('#')+1));
        listening = true;

        logger.info("Client listening on " + serverIp + ':' + std::to_string(port));
        threadAddressArg.ip = serverIp;
        threadAddressArg.port = port;

        pthread_t listenThread;
        pthread_create(&listenThread, nullptr, listen, (void*)&threadAddressArg);
      }
    } else if (std::count(cmd.begin(), cmd.end(), '#') == 2) {
      send(serverSocketFd, "List", 4, 0);
      peerLock = true;
      while (peerLock);
      auto [_, __, peerUsername] = parseTransferRequest(cmd);
      if (isConnectedToPeer) {
        close(connectingPeerSocketFd);
        if ((connectingPeerSocketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
          throw std::runtime_error("Socket Creation Error");
        }
      }
      connectToPeer(peerAddresses[peerUsername]);
      send(connectingPeerSocketFd, cmd.c_str(), cmd.size(), 0);
      return false;
    }
    send(serverSocketFd, cmd.c_str(), cmd.size(), 0);
    return true;
}

std::string SocketClient::commandLineInterface() {
    logger.cliOptions();
    std::string option;
    getline(std::cin, option);
    if (option.size() > 1) {
        logger.cliInvalidInput("Expected input size of 1 but got longer");
        return "";
    }
    std::string username, payerUsername, payeeUsername, port, amount;
    switch (option[0]) {
        case 'a':
            logger.cliPrompt("Enter username:");
            getline(std::cin, username);
            if (!validateUsername(username)) {
                logger.cliInvalidInput("Invalid username");
                return "";
            }

            logger.cliPrompt("Enter port:");
            getline(std::cin, port);
            if (!isNumber(port)) {
                logger.cliInvalidInput("Expected number input");
                return "";
            }
            return username + '#' + port;
        case 'b':
            logger.cliPrompt("Enter username:");
            getline(std::cin, username);

            if (!validateUsername(username)) {
                logger.cliInvalidInput("Invalid username");
                return "";
            }

            return "REGISTER#" + username;
        case 'c':
            return "List";
        case 'd':
            logger.cliPrompt("Enter payer:");
            getline(std::cin, payerUsername);

            if (!validateUsername(payerUsername)) {
                logger.cliInvalidInput("Invalid username");
                return "";
            }

            logger.cliPrompt("Enter payee:");
            getline(std::cin, payeeUsername);

            if (!validateUsername(payeeUsername)) {
                logger.cliInvalidInput("Invalid username");
                return "";
            }

            logger.cliPrompt("Enter amount:");
            getline(std::cin, amount);

            if (!isNumber(amount)) {
                logger.cliInvalidInput("Expected number input");
                return "";
            }

            return payerUsername + '#' + amount + '#' + payeeUsername;
        case 'e':
            return "Exit";
    }
    logger.cliInvalidInput("Invalid option");
    return "";
}