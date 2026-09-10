#include <iostream>
#include "../include/NetworkSocker.hpp"

int main()
{
    WSADATA wsaData;
    
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        return 1;
    
    {
        std :: string target = "45.33.32.156";
        std :: cout <<"Scanam porturile 78-82 pe "<<target<<"...\n";

        for(int i=78;i<=82;++i)
        {
            NetworkSocket scanner;
            if(scanner.connect_to(target,i,200))
            {
                std :: cout <<"[+] Portul " << i << " este DESCHIS!\n";
            }
            else
            {
                std :: cout <<"[-] Portul " << i << " este INCHIS!\n"; 
            }
        }
    }

    WSACleanup();
    return 0;

}

