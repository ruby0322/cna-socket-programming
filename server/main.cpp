#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SocketServer.hpp"

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

int main(int argc, char* argv[]) {
  OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, nullptr);
  SSL_library_init();
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();
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
