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

bool NetworkSocket :: connect_to(const std :: string &ip, int port)
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

    return res==0;

}
