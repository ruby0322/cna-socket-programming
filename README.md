# 💰 A Secure P2P Micropayment System

## Table of Contents

- [💰 A Secure P2P Micropayment System](#-a-secure-p2p-micropayment-system)
  - [Table of Contents](#table-of-contents)
  - [Project Overview 🚀](#project-overview-)
  - [Project Features ⭐](#project-features-)
    - [Socket Client](#socket-client)
    - [Socket Server](#socket-server)
    - [Shared](#shared)
  - [Project Structure 🏗️](#project-structure-️)
  - [Building and Running ▶️](#building-and-running-️)
  - [Usage 🧑‍💻](#usage-)
  - [Documentation](#documentation)
    - [Login \& Initiate P2P Listening](#login--initiate-p2p-listening)
    - [Register](#register)
    - [List Online Users](#list-online-users)
    - [Make a Micropayment](#make-a-micropayment)
    - [Exit and Disconnect](#exit-and-disconnect)


## Project Overview 🚀

This project is a secure Person-to-Person (P2P) micropayment system implemented in C++. It consists of both client and server components to facilitate secure micropayments between users. The system handles network communication using sockets and provides a command-line interface for user interaction.

## Project Features ⭐

### Socket Client

- **Responsibility:** The Socket Client component is responsible for connecting to the server and facilitating communication between the client and the server.

- **Key Functions:**
  1. **Initialization:** It sets up a connection to the server and manages communication through sockets.
  2. **Command-Line Interface:** Provides a user-friendly command-line interface for user interaction, allowing actions like logging in, registration, listing online users, micropayments, and exiting.
  3. **P2P Listening:** Enables the client to listen on a specific port for incoming P2P connections.
  4. **Message Transfer:** Communicates with the server by sending user commands and micropayment requests.

### Socket Server

- **Responsibility:** The Socket Server component serves as the central hub for handling client connections and micropayment transactions.

- **Key Functions:**
  1. **Initialization:** Sets up the server to listen for incoming connections from multiple clients.
  2. **Connection Handling:** Manages concurrent connections from multiple clients, enabling real-time communication.
  3. **User Management:** Handles user registration, ensuring unique and valid usernames.
  4. **Online User List:** Maintains and provides an online user list, which clients can request.
  5. **Micropayments:** Processes micropayment requests, verifies balances, and performs secure transfers between users.

### Shared

- **Purpose:** The Shared component contains shared code and utility classes used by both the Socket Client and Socket Server.

- **Key Elements:**
  1. **Logging:** Provides logging functionality to record events and interactions for auditing and debugging.
  2. **Thread Handling:** Manages multiple threads for concurrent execution of tasks across the entire project.
  3. **Utils:** Contains utility functions used across the project, aiding in parsing, validation, and other common tasks.

These three sections together create a secure Person2Person (P2P) micropayment system. The Socket Client and Socket Server components work in tandem to enable users to interact and perform micropayments securely, while the Shared component ensures code reusability and common utility functions across the project, including logging and thread handling.

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

## Documentation


### Login & Initiate P2P Listening

- **CLI Option**: `a`
- **Raw Message**: `REGISTER#<username>`
- **Success Response**: `100 OK`
- **Failure Response**: `210 FAIL

### Register

- **CLI Option**: `b`
- **Raw Message**: `REGISTER#<username>`
- **Success Response**:  List of online users and their details.
- **Failure Response**: `220 AUTH_FAIL

### List Online Users

- **CLI Option**: `c`
- **Raw Message**: `List`
- **Response**: List of online users and their details.

### Make a Micropayment

- **CLI Option**: `c`
- **Raw Message**: `<fromUser>#<amount>#<toUser>`
- **Success Response**: `100 OK`
- **Failure Response**:
  - `Transfer failed due to insufficient balance.`
  - `Transfer failed due to negative amount.`
  - `Transfer failed due to no permission.`
  - `Transfer failed due to invalid user.`

### Exit and Disconnect

- **CLI Option**: `d`
- **Raw Message**: `Exit`
- **Response**: `Bye`


Please refer to the source code for a more detailed understanding of the functionality.
