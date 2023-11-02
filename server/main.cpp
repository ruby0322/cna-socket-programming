#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SocketServer.hpp"

// Thread function to handle each client
void* handleClient(void* arg) {
    static int numClient = 0;
    ++numClient;
    int clientSocket = *((int*)arg);

    // std::cout << numClient << " client(s) is/are currently connected." << std::endl;
    std::cout << "New client connected!" << std::endl;
    while (true) {
      ;
    }
    
    close(clientSocket);
    --numClient;
    std::cout << "A client has disconnected!" << std::endl;
    // cout << numClient << " client(s) is/are currently connected." << std::endl;
    return nullptr;
}

void* handleClient(void* arg); 

int main(int argc, char* argv[]) {

  SocketServer server;
  server.listen(handleClient);

  return 0;
}
