#include <iostream>
#include "../include/NetworkSocker.hpp"

int main()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData)!= 0){
        std :: cerr <<"Ëroare la initializare WSAStartup!\n";
        return 1;
    }

    //folosim {} ca scop izolant pentru ca obocetul scanner sa fie distrus
    //iar destrutorul sa inchida socket ul inainte de WSACleanup()
    {
        NetworkSocket scanner_socket;
        std :: string target_ip = "45.33.32.156";
        int target_port=80;

        std :: cout <<"Verificam " << target_ip << "...\n";

        if(scanner_socket.connect_to(target_ip,target_port))
        {
            std :: cout <<"Portul "<<target_port << " este DESCHIS!\n";

        }
        else
        {
            std :: cout <<"Portul " << target_port << " este INCHIS!\n";
        }


    }

    WSACleanup();
    return 0;

}

