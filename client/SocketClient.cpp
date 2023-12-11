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

std::string SocketClient::serverPublicKey;
std::string SocketClient::publicKey;
std::string SocketClient::privateKey;
std::string SocketClient::peerPublicKey;

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
    
    // Generate RSA key pair for the client
    RSA* rsa = RSA_generate_key(BUFFER_SIZE, RSA_F4, nullptr, nullptr);
    if (!rsa) {
        throw std::runtime_error("Error generating RSA key pair.");
    }
    publicKey = getRSAPublicKeyString(rsa);
    privateKey = getRSAPrivateKeyString(rsa);
    RSA_free(rsa);
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

    send(serverSocketFd, publicKey.c_str(), publicKey.length(), 0);

    char buffer[BUFFER_SIZE] = { 0 };
    ssize_t bytesRead = recv(serverSocketFd, buffer, BUFFER_SIZE, 0);
    serverPublicKey = std::string(buffer, buffer+bytesRead);

    while (up) {
      char buffer[BUFFER_SIZE] = { 0 };
      bytesRead = recv(serverSocketFd, buffer, BUFFER_SIZE, 0);
      std::string rawMessage = std::string(buffer, buffer+bytesRead);
      logger.rawMessage(rawMessage);
      std::string message = decryptMessage(rawMessage, privateKey);
      logger.message(serverIp + ':' + std::to_string(serverPort), message);
      if (std::count(message.begin(), message.end(), '#') > 0) {
        updatePeerPorts(message);
      }
      messageLock = false;
    }
    return nullptr;
}

void SocketClient::sendMessageToPeer(Address addr, const std::string& message) {
    sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(addr.port);
    peerAddr.sin_addr.s_addr = inet_addr(addr.ip.c_str());
    if (::connect(connectingPeerSocketFd, (sockaddr *)&peerAddr, sizeof(peerAddr)) == -1) {
      std::cout << "failed to connect to peer " << addr.toString() << std::endl;
      throw std::runtime_error("Connection Error");
    }

    send(connectingPeerSocketFd, publicKey.c_str(), publicKey.length(), 0);

    char buffer[BUFFER_SIZE] = { 0 };
    ssize_t bytesRead = recv(connectingPeerSocketFd, buffer, BUFFER_SIZE, 0);
    peerPublicKey = std::string(buffer, buffer+bytesRead);
    __debug(peerPublicKey);

    sendEncryptedMessage(connectingPeerSocketFd, message, peerPublicKey);

    isConnectedToPeer = true;
    logger.info("Connected to peer " + addr.toString());

    close(connectingPeerSocketFd);
    if ((connectingPeerSocketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      throw std::runtime_error("Socket Creation Error");
    }
}

void* SocketClient::onPeerConnect(void* arg) {
    connectingPeerSocketFd = *((int*)arg);
    Address addr = getAddr(connectingPeerSocketFd);
    if (addr.ip == "0.0.0.0") {
        close(connectingPeerSocketFd);
        return nullptr;
    }
    logger.info("New connection from peer " + addr.toString());

    // exchange public keys
    
    send(connectingPeerSocketFd, publicKey.c_str(), publicKey.length(), 0);

    char buffer[BUFFER_SIZE] = { 0 };
    ssize_t bytesRead = recv(connectingPeerSocketFd, buffer, BUFFER_SIZE, 0);
    peerPublicKey = std::string(buffer, buffer+bytesRead);
    __debug(peerPublicKey);
    isConnectedToPeer = true;

    // while (up) {
    std::fill(buffer, buffer+BUFFER_SIZE, 0);
    bytesRead = recv(connectingPeerSocketFd, buffer, BUFFER_SIZE, 0);
    std::string raw(buffer, buffer+bytesRead);
    logger.rawMessage(raw);

    std::string msg = decryptMessage(raw, privateKey);
    logger.message(addr.toString(), msg);
    
    logger.info("Forwarding encryted peer message to server...");
    sendEncryptedMessage(serverSocketFd, msg, serverPublicKey);
    logger.peerDisconnection(addr.toString());
    close(connectingPeerSocketFd);
    // }
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
      sendEncryptedMessage(serverSocketFd, "List", serverPublicKey);
      peerLock = true;
      while (peerLock);
      auto [_, __, peerUsername] = parseTransferRequest(cmd);
      __debug(cmd);
      sendMessageToPeer(peerAddresses[peerUsername], cmd);
      return false;
    }
    sendEncryptedMessage(serverSocketFd, cmd, serverPublicKey);
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

