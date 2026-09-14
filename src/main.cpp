#include <iostream>
#include <string>
#include "../include/NetworkSocket.hpp"
#include <chrono>
#include "../include/PortScanner.hpp"
#include "../include/ResultExporter.hpp"


void print(const char* program_name){
    std::cout <<"Utilizare: " <<program_name << " -i <IP> -p <start-end> [-t <timeout_ms>] [--threads <nr_threaduri>]\n\n";
    std::cout <<"Optiuni:\n";
    std::cout <<"  -i, --ip        Adresa IP tinta (obligatoriu)\n";
    std::cout <<"  -p, --ports     Intervalul dfe porturi, ex: 1-1024 (obligatoriu)\n";
    std::cout <<"  -t, --timeout   Timeout per port in ms (implicit 500 ms)\n";
    std::cout << "  --threads      Numărul de thread-uri paralele (implicit: 50)\n";
    std::cout << "  -h, --help     Afișează acest mesaj de ajutor\n\n";
    std::cout << "Exemplu:\n";
    std::cout << "  " << program_name << " -i 45.33.32.156 -p 1-1000 -t 300 --threads 100\n";

}

int main(int argc, char* argv[])
{
    if(argc<2){
        print(argv[0]);
        return 1;
    } 

    std::string target_ip;
    std::string output_file;
    int start_port = 1;
    int end_port = 1024;
    int timeout_ms = 500;
    int thread_count = 50;

    for(int i = 1; i<argc;++i){
        std::string arg=argv[i];

        if(arg == "-h" || arg == "--help"){
            print(argv[0]);
            return 0;
        }else if ((arg == "-i" || arg == "--ip") && i + 1 < argc) {
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
        }else if((arg == "-o") || (arg == "--output") && i + 1 < argc){
            output_file = argv[++i];
        }
    }

    if (target_ip.empty()) {
        std::cerr << "[!] Eroare: Adresa IP este obligatorie!\n\n";
        print(argv[0]);
        return 1;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[!] Eroare la inițializarea Winsock!\n";
        return 1;
    }

    {
        std::cout << "========================================\n";
        std::cout << " Target:   " << target_ip << "\n";
        std::cout << " Porturi:  " << start_port << " - " << end_port << "\n";
        std::cout << " Threads:  " << thread_count << "\n";
        std::cout << " Timeout:  " << timeout_ms << " ms\n";
      
        
        if(!output_file.empty()){
            std::cout<<"Output : " << output_file << "\n";
        }
        std::cout << "========================================\n\n";
        auto start_time = std::chrono::high_resolution_clock::now();

        PortScanner scanner(target_ip, start_port, end_port, timeout_ms);
        scanner.scan(thread_count);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        auto open_ports = scanner.get_open_ports();

        std::cout << "\n----------------------------------------\n";
        std::cout << "Scanare finalizata in " << duration / 1000.0 << " secunde!\n";
        std::cout << "Total porturi deschise gasite: " << open_ports.size() << "\n";

        if(!output_file.empty()){
            bool success = false;
            if(output_file.rfind(".csv") != std::string::npos){
                success = ResultExporter :: to_csv(output_file,open_ports);
            }else{
                success = ResultExporter :: to_json(output_file,target_ip,open_ports);
            }

            if(success){
                std::cout<<"[+] Rezultatele au fost salvate in: "<<output_file << "\n";
                
            }else{
                std::cerr <<"[!] Eroare la salvarea fisierului!\n";
            }
        }
    }

    WSACleanup();
    return 0;

}

