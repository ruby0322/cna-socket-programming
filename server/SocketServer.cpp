#ifndef SOCKET_SERVER_HPP
  #include "SocketServer.hpp"
#endif

SocketServer::SocketServer() {
    setup();
}

SocketServer::~SocketServer() {

}

void SocketServer::setup() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000); 
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    bind(serverSocketFd, (sockaddr*)&serverAddress, sizeof(serverAddress));
}

void SocketServer::listen(void* (*clientHandler)(void*)) {
    int newSocket;
    sockaddr_in clientAddress;
    ::listen(serverSocketFd, MAX_CLIENTS);
    while (true) {
        int clientSize = sizeof(clientAddress);
        newSocket = accept(serverSocketFd, (struct sockaddr*)&clientAddress, (socklen_t*)&clientSize);

        pthread_t threadId;
        pthread_create(&threadId, NULL, clientHandler, &newSocket);
    }
}