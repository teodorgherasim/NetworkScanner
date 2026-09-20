# C++ Network Security Scanner

A TCP port scanner written in C++17 for Windows using Winsock2.

I built this project to practice low-level socket programming, C++ resource management (RAII), and setting up a clean CMake build system with MinGW on Windows.
## Key Features

- **High-Concurrency Scanning Engine:** Custom `ThreadPool` implementation and thread-safe worker logic using Winsock2 non-blocking patterns.
- **Service & Banner Grabbing:** Automated mapping of common application layer protocols (HTTP, SSH, FTP, MySQL, etc.) and banner extraction.
- **Dual Interface:**
- **CLI:** Command-line options for targeted scanning, threading parameters, and file exports.
- **REST API & Web UI:** Embedded HTTP server (`cpp-httplib`) serving a vanilla JS dashboard and exposing structured JSON endpoints.
- **Data Persistence:** Integrated `ResultExporter` supporting formatted JSON and CSV outputs.


## Performance & Scaling Benchmarks (ThreadPool Engine)

The following benchmark was executed locally on `127.0.0.1` across a range of **1,000 TCP ports** with a connection timeout of `200ms` using the custom `ThreadPool`:

| Threads | Duration (s) | Throughput (ports/s) | Speedup | Notes / System Behavior |
| :---: | :---: | :---: | :---: | :--- |
| **1** | `206.00 s` | `4.85` | `1.0x` | Baseline sequential execution bound by I/O timeout |
| **10** | `20.97 s` | `47.68` | `9.82x` | Linear scaling with minimal queue contention |
| **25** | `8.52 s` | `117.34` | `24.18x` | Efficient Winsock socket reuse |
| **50** | `4.22 s` | `236.80` | `48.82x` | Optimal operational point for local network sweeps |
| **100** | `2.17 s` | `461.21` | `94.93x` | Sub-2.5 second scan completion |
| **200** | `1.13 s` | `882.45` | `182.30x` | High queue processing throughput |
| **500** | **`0.53 s`** | **`1870.61`** | **`388.68x`** | Maximum throughput limit under local network conditions |

---


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


## Architecture Overview

```text
[ CLI / Web UI ] ──► [ ApiServer / Main ] ──► [ PortScanner Engine ] ──► [ ThreadPool ]
                                                                                │
                                                                    ┌───────────┴───────────┐
                                                                    ▼                       ▼
                                                             [ NetworkSocket ]    [ ResultExporter ]
```
## Project Structure

```text
network_scanner/
├── include/
│   ├── ApiServer.hpp       # Embedded HTTP server & REST API wrapper
│   ├── NetworkSocket.hpp   # Non-blocking Winsock2 socket wrapper
│   ├── PortScanner.hpp     # Multi-threaded scanning engine interface
│   ├── ResultExporter.hpp  # Export handler for JSON and CSV formats
│   └── ThreadPool.hpp      # Header-only C++17 thread pool implementation
├── src/
│   ├── ApiServer.cpp       # REST API endpoints & route handling
│   ├── NetworkSocket.cpp   # Low-level network socket operations
│   ├── PortScanner.cpp     # Core scanning logic & task dispatching
│   ├── ResultExporter.cpp  # File writing implementation
│   └── main.cpp            # CLI argument parser & application entry point
├── public/
│   └── index.html          # Web UI Dashboard (HTML/CSS/JS)
├── CMakeLists.txt          # CMake build configuration
└── README.md               # Documentation & benchmark report


