#include <iostream>
#include "../include/NetworkSocket.hpp"
#include <chrono>
#include "../include/PortScanner.hpp"

int main()
{
    WSADATA wsaData;
    
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        return 1;
    
    {
        std :: string target = "45.33.32.156";
        int start_port = 1;
        int end_port = 1024;
        int thread_count = 500;

        std :: cout << "Scanam porturile "<<start_port<<"-"<<end_port<<" pe "<< target << " folosind " <<thread_count <<" threads....\n";
        auto start_time = std :: chrono:: high_resolution_clock :: now();

        PortScanner scanner(target, start_port, end_port, 500);
        scanner.scan(thread_count);

        auto end_time = std :: chrono :: high_resolution_clock :: now();
        auto duration = std :: chrono :: duration_cast < std :: chrono :: milliseconds>(end_time-start_time).count();

        std::cout << "\n----------------------------------------\n";
        std::cout << "Scanare finalizata in " << duration / 1000.0 << " secunde!\n";
        std::cout << "Total porturi deschise gasite: " << scanner.get_open_ports().size() << "\n";
    }

    WSACleanup();
    return 0;

}

