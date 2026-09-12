# C++ Network Security Scanner

A TCP port scanner written in C++17 for Windows using Winsock2.

I built this project to practice low-level socket programming, C++ resource management (RAII), and setting up a clean CMake build system with MinGW on Windows.
## Features

- **RAII Socket Abstraction:** Custom `NetworkSocket` class that manages socket lifecycle and handles resource cleanup automatically (`closesocket`).
- **TCP Connection Testing:** Synchronous connection testing against IPv4 targets using raw sockets.
- **Clean Build System:** Configured with CMake and Ninja for fast compilation.


## Build and Run Prerequisites

- MinGW-w64 (g++)
- CMake (3.16+)
- Ninja

## Commands PowerShell
- Generate build files
   - cmake -B build -G Ninja

-  Compile
   - cmake --build build

-  Run executable
   - .\build\scanner.exe



## Project Structure

```text
network_scanner/
├── include/
│   └── NetworkSocket.hpp   # Socket class declaration
├── src/
│   ├── NetworkSocket.cpp   # Socket implementation
│   └── main.cpp            # Entry point & test suite
└── CMakeLists.txt          # Build configuration



