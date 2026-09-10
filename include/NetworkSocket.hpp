#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>


class NetworkSocket {
    private:
        SOCKET m_socket;
    public:
        NetworkSocket();
        ~NetworkSocket();

        //Blocam copierea pentru a preveni ichiderea dubla a socket-ului
        NetworkSocket(const NetworkSocket&) = delete;
        NetworkSocket& operator=(const NetworkSocket&) = delete;

        bool connect_to(const std:: string &ip, int port, int timeout_ms = 200);
        bool is_valid() const;

};