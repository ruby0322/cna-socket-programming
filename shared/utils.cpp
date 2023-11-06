#include "utils.hpp"

bool isNumber(std::string s) {
  for (auto& c : s)
    if (!isdigit((c)))
      return false;
  return true;
}

Address getAddr(int clientSocket) {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    getpeername(clientSocket, (sockaddr*)&clientAddr, &clientLen);

    char clientIP[256]; 
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, 256);
    int len = strlen(clientIP);

    std::string ip;

    for (int i = 0; i < len; ++i)
        ip += clientIP[i];

    return { ip, ntohs(clientAddr.sin_port) };
}

std::tuple<std::string, int, std::string> parseTransferRequest(std::string req) {
    size_t firstOccurence = req.find('#');
    size_t secondOccurence = req.find('#', firstOccurence+1);
    return { req.substr(0, firstOccurence), std::stoi(req.substr(firstOccurence+1, secondOccurence-firstOccurence-1)), req.substr(secondOccurence+1)};
}

std::tuple<std::string, std::string, int> parseOnlineListLine(std::string line) {
  size_t firstOccurence = line.find('#');
  size_t secondOccurence = line.find('#', firstOccurence+1);
  std::string username = line.substr(0, firstOccurence);
  std::string ip = line.substr(firstOccurence+1, secondOccurence-firstOccurence-1);
  int port = std::stoi(line.substr(secondOccurence+1));
  return { username, ip, port };
}

std::vector<std::tuple<std::string, std::string, int>> parseOnlineList(std::string list) {
  std::vector<std::tuple<std::string, std::string, int>> parsedLines;
  std::vector<std::string> lines;
  for (size_t curr = list.find('\n'); curr != std::string::npos; curr = list.find('\n')) {
    lines.push_back(list.substr(0, curr));
    if (curr != std::string::npos) {
      list = list.substr(curr+1);
    }
  }
  for (auto& line : lines) {
    parsedLines.push_back(parseOnlineListLine(line));
  }
  return parsedLines;
}

bool validateUsername(std::string username) {
  if (username.find(' ') != std::string::npos || username.find('#') != std::string::npos) {  // 1. No spaces and #
    return false;
  }
  if (isNumber(username)) {  // 2. No pure number
    return false;
  }
  if (username == "[unauthed]") { // 3. Not reserved word
    return false;
  }
  return true;
}