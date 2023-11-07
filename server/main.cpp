#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SocketServer.hpp"

int main(int argc, char* argv[]) {
  char mode = 'd';
  SocketServer bankServer(mode, "BankServer");
  if (argc < 2) {
    std::cout << "Usage: ./<filename> <server-port> [-options]" << std::endl;
    return -1;
  } else {
    if (argc == 3) {
      mode = argv[2][1];
    }
    bankServer.setPort(std::stoi(std::string(argv[1])));
  }
  bankServer.listen();
  return 0;
}
