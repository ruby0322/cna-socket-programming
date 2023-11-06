# A Secure Person2Person (P2P) Micropayment System

## Project Overview

This project is a secure Person-to-Person (P2P) micropayment system implemented in C++. It consists of both client and server components to facilitate secure micropayments between users. The system handles network communication using sockets and provides a command-line interface for user interaction.

## Project Structure

The project directory has the following structure:

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

- The `client/` directory contains the client component source files.
- The `server/` directory contains the server component source files.
- The `shared/` directory contains common utility and shared code used by both the client and server components.
- The root directory includes the project's README, .gitignore, and Makefile.

## Building and Running

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

## Usage

The client provides a command-line interface for interaction. You can use various commands to interact with the micropayment system.


- To login and initiate P2P listening on a specific port, enter: `a`

- To register a user, enter: `b`

- To list online users, enter: `c`

- To make a micropayment, enter: `d`

- To exit the client, enter: `a`


Please refer to the source code for a more detailed understanding of the functionality.