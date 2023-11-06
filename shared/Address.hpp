#define ADDRESS_HPP
#include <string>

class Address {
public:
  std::string ip;
  int port;
  std::string toString() const;
};