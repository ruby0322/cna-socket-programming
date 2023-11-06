#ifndef SOCKET_CLIENT_HPP
  #include "SocketClient.hpp"
#endif

int main(int argc, char const *argv[]) {

  SocketClient client("BankClient");
  client.run();

  return 0;
}