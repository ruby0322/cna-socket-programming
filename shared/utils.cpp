#include "utils.hpp"

bool isNumber(const std::string& s) {
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

std::tuple<std::string, int, std::string> parseTransferRequest(const std::string& req) {
    size_t firstOccurence = req.find('#');
    size_t secondOccurence = req.find('#', firstOccurence+1);
    return { req.substr(0, firstOccurence), std::stoi(req.substr(firstOccurence+1, secondOccurence-firstOccurence-1)), req.substr(secondOccurence+1)};
}

std::tuple<std::string, std::string, int> parseOnlineListLine(const std::string& line) {
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

bool validateUsername(const std::string& username) {
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

std::string getRSAPublicKeyString(RSA* rsa) {
    BIO *bioPublic = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bioPublic, rsa);
    char buffer[BUFFER_SIZE] = { 0 };
    BIO_read(bioPublic, buffer, BUFFER_SIZE);
    BIO_free(bioPublic);
    return std::string(buffer);
}

std::string getRSAPrivateKeyString(RSA* rsa) {
    BIO *bioPublic = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bioPublic, rsa, nullptr, nullptr, 0, nullptr, nullptr);
    char buffer[BUFFER_SIZE] = { 0 };
    BIO_read(bioPublic, buffer, BUFFER_SIZE);
    BIO_free(bioPublic);
    return std::string(buffer);
}

RSA* getRSAPublicKey(const std::string& keyString) {
    BIO *bioRSA = BIO_new_mem_buf(keyString.c_str(), strlen(keyString.c_str()));
    RSA* rsa = PEM_read_bio_RSAPublicKey(bioRSA, nullptr, nullptr, nullptr);
    if (!rsa) {
      std::cout << "Error: RSA public key" << std::endl;
      printSSLError();
      return nullptr;
    }
    BIO_free(bioRSA);
    return rsa;
}

RSA* getRSAPrivateKey(const std::string& keyString) {
    BIO *bioRSA = BIO_new_mem_buf(keyString.c_str(), strlen(keyString.c_str()));
    RSA* rsa = PEM_read_bio_RSAPrivateKey(bioRSA, nullptr, nullptr, nullptr);
    if (!rsa) {
      std::cout << "Error: RSA private key" << std::endl;
      printSSLError();
      return nullptr;
    }
    BIO_free(bioRSA);
    return rsa;
}

std::string encryptMessage(const std::string& plaintext, const std::string& publicKey) {
    RSA* rsa = getRSAPublicKey(publicKey);
    // Convert the plaintext to unsigned char buffer
    const unsigned char* inputBuffer = reinterpret_cast<const unsigned char*>(plaintext.c_str());
    int inputLength = static_cast<int>(plaintext.length());

    // Determine the size of the output buffer
    int outputLength = RSA_size(rsa);
    
    // Allocate a C-style buffer
    unsigned char* outputBuffer = new unsigned char[outputLength]{0};

    // Perform the encryption
    int result = RSA_public_encrypt(inputLength, inputBuffer, outputBuffer, rsa, RSA_PKCS1_PADDING);

    if (result == -1) {
        // Handle encryption error
        std::cout << "Error: Encryption Failed." << std::endl;
        printSSLError();
        delete[] outputBuffer;
        RSA_free(rsa);
        return "";   
    }

    // Convert the encrypted data to a string for storage/transmission
    std::string encryptedMessage(outputBuffer, outputBuffer + result);

    // Clean up the allocated buffer
    delete[] outputBuffer;
    RSA_free(rsa);

    return encryptedMessage;
}

std::string decryptMessage(const std::string& encryptedMessage, const std::string& privateKey) {
    RSA* rsa = getRSAPrivateKey(privateKey);

    // Convert the encrypted data to unsigned char buffer
    const unsigned char* inputBuffer = reinterpret_cast<const unsigned char*>(encryptedMessage.c_str());
    int inputLength = static_cast<int>(encryptedMessage.length());

    // Determine the size of the output buffer
    int outputLength = RSA_size(rsa);
    
    // Allocate a C-style buffer
    unsigned char* outputBuffer = new unsigned char[outputLength]{0};

    // Perform the decryption
    int result = RSA_private_decrypt(inputLength, inputBuffer, outputBuffer, rsa, RSA_PKCS1_PADDING);

    if (result == -1) {
        // Handle decryption error
        std::cout << "Error: Decryption Failed." << std::endl;
        printSSLError();
        delete[] outputBuffer;
        return "";
    }

    // Convert the decrypted data to a string
    std::string decryptedMessage(outputBuffer, outputBuffer + result);

    // Clean up the allocated buffer
    delete[] outputBuffer;
    RSA_free(rsa);

    return decryptedMessage;
}

void printRSAPublicKey(RSA* rsa) {
    if (rsa == nullptr) {
        std::cerr << "RSA key is null." << std::endl;
        return;
    }

    BIO* bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        std::cerr << "Failed to create BIO." << std::endl;
        return;
    }

    if (PEM_write_bio_RSAPublicKey(bio, rsa) == 1) {
        char* buffer;
        long length = BIO_get_mem_data(bio, &buffer);
        if (length > 0) {
            std::cout << "RSA Public Key:" << std::endl;
            std::cout.write(buffer, length);
            std::cout << std::endl;
        } else {
            std::cerr << "Failed to get key data from BIO." << std::endl;
        }
    } else {
        std::cerr << "Failed to write RSA public key to BIO." << std::endl;
    }

    BIO_free(bio);
}

void sendEncryptedMessage(int socketFd, const std::string& message, const std::string& publicKey) {
    std::string encryptedMessage = encryptMessage(message, publicKey);
    __debug(encryptedMessage);
    send(socketFd, encryptedMessage.c_str(), encryptedMessage.length(), 0);
}

void printSSLError() {
    unsigned long errCode = ERR_get_error();
    char errString[256] = { 0 };
    ERR_error_string(errCode, errString);
    fprintf(stderr, "OpenSSL Error: %s\n", errString);
}