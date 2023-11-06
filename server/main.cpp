#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SocketServer.hpp"

int main(int argc, char* argv[]) {
  char mode = 'd';
  if (argc > 1) {
    mode = argv[1][1];
  }
  SocketServer bankServer(mode, "BankServer");
  bankServer.listen();

  return 0;
}
