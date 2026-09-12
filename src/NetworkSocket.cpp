#include "NetworkSocket.hpp"

NetworkSocket :: NetworkSocket(){
    m_socket = INVALID_SOCKET;
    m_wsa_initialized=false;
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2),&wsaData) == 0){
        m_wsa_initialized=true;
    }


}

NetworkSocket :: ~NetworkSocket(){
    if(m_socket != INVALID_SOCKET){
        closesocket(m_socket);
    }
    if(m_wsa_initialized){
        WSACleanup();
    }
}

bool NetworkSocket :: is_valid()const
{
    return m_socket!=INVALID_SOCKET;
} 

bool NetworkSocket :: connect_to(const std :: string &ip, int port, int timeout_ms)
{
    if(!m_wsa_initialized) return false;



    if(is_valid())
    {
        close();
    }

    m_socket = socket(AF_INET,SOCK_STREAM,0);

    if(m_socket == INVALID_SOCKET)
    {
        return false;
    }

    u_long mode = 1;
    if(ioctlsocket(m_socket, FIONBIO, &mode) == SOCKET_ERROR) //sistemul de operare nu blocheaza firul de executie la apelarea connect 
    {
        close();
        return false;
    }
    
    sockaddr_in server_addr{}; //{}declara toti octetii cu 0
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(static_cast<u_short>(port)); //prevenim advertismente de compilare deoarece htos asteapta un intreg pe 16 biti

    if(inet_pton(AF_INET,ip.c_str(),&server_addr.sin_addr)<=0)
    {
        close();
        return false;
    }

    int res = connect(m_socket,reinterpret_cast<sockaddr*>(&server_addr),sizeof(server_addr));

    if(res == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if(err != WSAEWOULDBLOCK)
        {
            close();
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

    close();
    return false;

}

void NetworkSocket :: close(){
    if(m_socket != INVALID_SOCKET){
        closesocket(m_socket);
        m_socket= INVALID_SOCKET;
    }
}

std :: string NetworkSocket :: grab_banner(int timeout_ms){
    if(!is_valid())return "";

    const char* probe="HEAD / HTTP/1.1\r\nHost : target\r\n\r\n";
    send(m_socket,probe, static_cast<int>(strlen(probe)),0);

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(m_socket, &read_fds);

    timeval tv{};
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int sel_res = select(0,&read_fds,NULL,NULL,&tv);
    if(sel_res > 0 && FD_ISSET(m_socket,&read_fds)){
        char buffer[512] = {0};
        int bytes_recieved = recv(m_socket,buffer,static_cast<int>(sizeof(buffer)-1),0);

        if(bytes_recieved > 0){
            buffer[bytes_recieved] = '\0';
            std :: string banner(buffer);

            for(char &c : banner){
                if( c == '\r' || c == '\n') 
                    c = ' ';
            }
            return banner;


        }
    }

    return "No banner returned";
}


