#ifndef SOCKET_CLIENT_HPP
  #include "SocketClient.hpp"
#endif

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

int main(int argc, char const *argv[]) {
  OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, nullptr);
  SSL_library_init();
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();

  SocketClient bankClient("BankClient");
  if (argc < 3) {
    std::cout << "Usage: ./<filename> <server-ip> <server-port> [-options]" << std::endl;
    return -1;
  } else {
    bankClient.setServer(std::string(argv[1]), std::stoi(std::string(argv[2])));
  }
  bankClient.run();
  return 0;
}