#define SOCKET_SERVER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

class SocketServer {
private:
  const int MAX_CLIENTS = 100;
  const int BUFFER_SIZE = 1024;
  
  int serverSocketFd;
  sockaddr_in serverAddress;
public:
    SocketServer();
    ~SocketServer();
  void setup();
  void listen(void* (*clientHandler)(void*));
};