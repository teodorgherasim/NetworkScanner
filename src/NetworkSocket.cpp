#include "NetworkSocker.hpp"

NetworkSocket :: NetworkSocket(){
    m_socket = INVALID_SOCKET;
}

NetworkSocket :: ~NetworkSocket(){
    if(m_socket != INVALID_SOCKET){
        closesocket(m_socket);
    }
}

bool NetworkSocket :: is_valid()const
{
    return m_socket!=INVALID_SOCKET;
} 

bool NetworkSocket :: connect_to(const std :: string &ip, int port, int timeout_ms)
{
    if(is_valid())
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }

    m_socket = socket(AF_INET,SOCK_STREAM,0);

    if(m_socket == INVALID_SOCKET)
    {
        return false;
    }

    u_long mode = 1;
    if(ioctlsocket(m_socket, FIONBIO, &mode) == SOCKET_ERROR) //sistemul de operare nu blocheaza firul de executie la apelarea connect 
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    
    sockaddr_in server_addr{}; //{}declara toti octetii cu 0
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(static_cast<u_short>(port)); //prevenim advertismente de compilare deoarece htos asteapta un intreg pe 16 biti

    if(inet_pton(AF_INET,ip.c_str(),&server_addr.sin_addr)<=0)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return false;
    }

    int res = connect(m_socket,reinterpret_cast<sockaddr*>(&server_addr),sizeof(server_addr));

    if(res == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if(err != WSAEWOULDBLOCK)
        {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
            return false;
        }
    }
    else if(res == 0)
    {
        return true;
    }

    fd_set write_fds, err_fds;
    FD_ZERO(&write_fds);
    FD_ZERO(&err_fds);
    FD_SET(m_socket, &write_fds); //asteptam ca socket ul sa poata fi scris 
    FD_SET(m_socket, &err_fds); // asteptam erori 

    timeval tv{};
    tv.tv_sec = timeout_ms/1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int sel_res = select(0,NULL,&write_fds, &err_fds, &tv); // opreste executia doar pana la timeout_ms
    if(sel_res > 0 && FD_ISSET(m_socket, &write_fds) && !FD_ISSET(m_socket,&err_fds))
    {
        int so_error = 0;
        int len = sizeof(so_error);
        getsockopt(m_socket,SOL_SOCKET,SO_ERROR,reinterpret_cast<char*>(&so_error),&len); // confirmam ca socket ul s-a conectat cu succes


        if(so_error == 0)
        {
            return true;
        }
    }

    closesocket(m_socket);
    m_socket = INVALID_SOCKET;
    return false;

}
