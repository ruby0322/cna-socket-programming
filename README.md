# 💰 A Secure Person2Person (P2P) Micropayment System

## Project Overview 🚀

This project is a secure Person-to-Person (P2P) micropayment system implemented in C++. It consists of both client and server components to facilitate secure micropayments between users. The system handles network communication using sockets and provides a command-line interface for user interaction.

## Project Features ⭐

### Socket Client

1. **Initialization:** The Socket Client is responsible for connecting to the server and managing communication with it.

2. **Command-Line Interface:** It provides a command-line interface for user interaction. Users can log in, register, list online users, make micropayments, and exit using different commands.

3. **Connection:** The client connects to the server using a socket and sends and receives messages to and from the server.

4. **Listening for P2P:** The client can initiate P2P listening on a specific port, allowing other users to connect to it for micropayments.

5. **Communication:** It sends user commands and micropayment requests to the server, which processes these requests.

6. **Thread Handling:** The client manages multiple threads for listening to incoming connections and handling user commands simultaneously.

### Socket Server

1. **Initialization:** The Socket Server initializes and sets up the server to listen for incoming connections.

2. **Connection Handling:** It listens for incoming connections from multiple clients using sockets and handles these connections concurrently.

3. **User Registration:** The server handles user registration, ensuring that usernames are unique and valid.

4. **User Login:** When a user logs in, the server tracks their connection and allows them to interact with other users.

5. **Online User List:** It maintains an online user list, which can be requested by clients, and sends this list to clients when requested.

6. **Micropayments:** The server processes micropayment requests from clients, checks balances, and performs transfers between users.

7. **Thread Handling:** Like the client, the server manages multiple threads for handling multiple client connections simultaneously.

8. **Logging:** Both the client and server components have logging functionality to record various events and interactions for debugging and auditing.

These components work together to provide a secure Person2Person (P2P) micropayment system. The client initiates and requests micropayments, while the server manages user registration, online user lists, and the actual micropayment transactions. The socket communication enables real-time interaction between clients and the server.

## Project Structure 🏗️

The project directory has the following structure:

```
/
|-- client/
|   |-- main.cpp
|   |-- SocketClient.cpp
|   |-- SocketClient.hpp
|
|-- server/
|   |-- main.cpp
|   |-- SocketServer.cpp
|   |-- SocketServer.hpp
|   |-- User.cpp
|   |-- User.hpp
|
|-- shared/
|   |-- Address.cpp
|   |-- Address.hpp
|   |-- Logger.cpp
|   |-- Logger.hpp
|   |-- utils.cpp
|   |-- utils.hpp
|
|-- README.md
|-- .gitignore
|-- Makefile
```

- The `client/` directory contains the client component source files.
- The `server/` directory contains the server component source files.
- The `shared/` directory contains common utility and shared code used by both the client and server components.
- The root directory includes the project's README, .gitignore, and Makefile.

## Building and Running ▶️

1. Make sure you have a C++ compiler installed (e.g., Clang or GCC) on your system.

2. Navigate to the project directory in your terminal.

3. Run the following command to compile the project:

```shell
make
```

This will build the client and server executables.

4. To run the client, use the following command:

```shell
./client/client
```

5. To run the server, use the following command:

```shell
./server/server
```

## Usage 🧑‍💻

The client provides a command-line interface for interaction. You can use various commands to interact with the micropayment system.

- 🆔 To login and initiate P2P listening on a specific port, enter: `a` 

- 📝 To register a user, enter: `b` 

- 📊 To list online users, enter: `c` 

- 💸 To make a micropayment, enter: `d` 

- 🚪 To exit the client, enter: `a` 

Please refer to the source code for a more detailed understanding of the functionality.