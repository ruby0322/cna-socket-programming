#ifndef SOCKET_CLIENT_HPP
  #include "SocketClient.hpp"
#endif

int main(int argc, char const *argv[]) {

  SocketClient client("BankClient");
  if (argc < 3) {
    std::cout << "Usage: ./<filename> <server-ip> <server-port> [-options]" << std::endl;
    return -1;
  } else {
    client.setServer(std::string(argv[1]), std::stoi(std::string(argv[2])));
  }
  client.run();
  return 0;
}