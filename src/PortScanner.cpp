#include <iostream>
#include "PortScanner.hpp"
#include "NetworkSocket.hpp"
#include <thread>


PortScanner :: PortScanner(std :: string target_ip, int start_port, int end_port, int timeout_ms)
    :m_target_ip(std::move(target_ip)),
    m_start_port(start_port),
    m_end_port(end_port),
    m_timeout_ms(timeout_ms),
    m_current_port(start_port)
{   
}

void PortScanner :: worker_thread()
{
    while(true)
    {
        int port = m_current_port.fetch_add(1);
        if(port > m_end_port)
        {
            break;
        }

        NetworkSocket socket;
        if(socket.connect_to(m_target_ip,port,m_timeout_ms))
        {
            std :: lock_guard < std:: mutex> lock(m_mutex);
            m_open_ports.push_back(port);
            std :: cout << "[+] Portul " << port << " este DESCHIS!\n";
        }
    }
}


void PortScanner :: scan(size_t thread_count)
{
    m_current_port = m_start_port;
    m_open_ports.clear();

    std :: vector<std::thread> threads;
    threads.reserve(thread_count);

    for(size_t i = 0; i< thread_count; ++i)
    {
        threads.emplace_back(&PortScanner :: worker_thread, this);
    }

    for(auto& t : threads)
    {
        if(t.joinable())
        {
            t.join();
        }
    }
}



std :: vector<int> PortScanner :: get_open_ports() const
{
    std :: lock_guard <std::mutex> lock(m_mutex);
    return m_open_ports;
}