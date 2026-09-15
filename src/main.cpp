#include <iostream>
#include <string>
#include "../include/NetworkSocket.hpp"
#include <chrono>
#include "../include/PortScanner.hpp"
#include "../include/ResultExporter.hpp"
#include "../include/ApiServer.hpp"

int main(int argc, char* argv[]) {
    auto print_usage = [](const char* prog_name) {
        std::cout << "Utilizare CLI: " << prog_name << " -i <IP> -p <start-end> [-t <timeout_ms>] [--threads <nr_threaduri>] [-o <fisier>]\n";
        std::cout << "Utilizare API: " << prog_name << " --server [port]\n\n";
    };

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    bool is_server_mode = false;
    int server_port = 8080;

    std::string target_ip;
    std::string output_file;
    int start_port = 1;
    int end_port = 1024;
    int timeout_ms = 500;
    int thread_count = 50;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--server") {
            is_server_mode = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                server_port = std::stoi(argv[++i]);
            }
        } else if ((arg == "-i" || arg == "--ip") && i + 1 < argc) {
            target_ip = argv[++i];
        } else if ((arg == "-p" || arg == "--ports") && i + 1 < argc) {
            std::string ports_str = argv[++i];
            size_t dash_pos = ports_str.find('-');
            if (dash_pos != std::string::npos) {
                start_port = std::stoi(ports_str.substr(0, dash_pos));
                end_port = std::stoi(ports_str.substr(dash_pos + 1));
            }
        } else if ((arg == "-t" || arg == "--timeout") && i + 1 < argc) {
            timeout_ms = std::stoi(argv[++i]);
        } else if (arg == "--threads" && i + 1 < argc) {
            thread_count = std::stoi(argv[++i]);
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output_file = argv[++i];
        }
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[!] Eroare la inițializarea Winsock!\n";
        return 1;
    }

    if (is_server_mode) {
        ApiServer server(server_port);
        server.start();
    } else {
        if (target_ip.empty()) {
            std::cerr << "[!] Eroare: Adresa IP este obligatorie în modul CLI!\n\n";
            print_usage(argv[0]);
            WSACleanup();
            return 1;
        }

        PortScanner scanner(target_ip, start_port, end_port, timeout_ms);
        scanner.scan(thread_count);

        auto open_ports = scanner.get_open_ports();

        if (!output_file.empty()) {
            if (output_file.rfind(".csv") != std::string::npos) {
                ResultExporter::to_csv(output_file, open_ports);
            } else {
                ResultExporter::to_json(output_file, target_ip, open_ports);
            }
        }
    }

    WSACleanup();
    return 0;
}

