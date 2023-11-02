#include <iostream>
#include <stdio.h>  
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8000 

int main(int argc, char const *argv[]) {

  int sock = 0; 
  struct sockaddr_in serv_addr;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
       
  // Convert IPv4 address from text to binary 
  if(inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr)<=0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }
   
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }

  // Send and receive data...
  while (true) {
    std::cout << "connected to server" << std::endl;
  }
  close(sock);
  return 0;
}