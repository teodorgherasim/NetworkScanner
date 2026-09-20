#include <iostream>
#include "PortScanner.hpp"
#include "NetworkSocket.hpp"
#include "ThreadPool.hpp"
#include <unordered_map>

static std :: string get_service_name(int port)
{
    static const std :: unordered_map<int,std::string> unknown_ports = {
        {21,"FTP"},{22,"SSH"},{23,"Telney"},{25,"SMTP"},{53,"DNS"},
        {80,"HTTP"},{110,"POP3"},{143,"IMAP"},{443,"HTTPS"},{3306,"MySQL"},
        {5432,"PostgreSQL"},{8080,"HTTP-PROXY"},{8443,"HTTPS-ALT"}
    };

    auto it = unknown_ports.find(port);
    return (it != unknown_ports.end()) ? it->second : "UNKNOWN";
}

PortScanner::PortScanner(std::string target_ip, int start_port, int end_port, int timeout_ms)
    : m_target_ip(std::move(target_ip)),
      m_start_port(start_port),
      m_end_port(end_port),
      m_timeout_ms(timeout_ms),
      m_current_port(start_port)
{
}

void PortScanner::worker_thread()
{
    while (true)
    {
        int port = m_current_port.fetch_add(1);
        if (port > m_end_port)
        {
            break;
        }

        NetworkSocket socket;
        if (socket.connect_to(m_target_ip, port, m_timeout_ms))
        {
            std::string banner = socket.grab_banner(m_timeout_ms);

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_open_ports.emplace_back(PortResult{port,banner});


                std::cout << "[+] Portul " << port << " este DESCHIS!\n";
                if (!banner.empty() && banner != "No banner returned")
                {
                    std::cout << " | Banner: " << banner;
                }
                std::cout << "\n";
            }
        }
    }
}

void PortScanner::scan(size_t thread_count)
{
    m_open_ports.clear();

    ThreadPool pool(thread_count);
    std::vector<std::future<void>> futures;
    int total_ports = m_end_port - m_start_port +1;
    futures.reserve(total_ports);

    for(int port = m_start_port; port <= m_end_port; ++port){
        futures.push_back(pool.enqueue([this, port](){
            NetworkSocket socket;
            if(socket.connect_to(m_target_ip,port,m_timeout_ms)){
                std::string banner = socket.grab_banner(m_timeout_ms);
                std::string service = get_service_name(port);

                std::lock_guard<std::mutex> lock(m_mutex);
                m_open_ports.emplace_back(PortResult{port,service,banner});
                std::cout << "[+] Portul "<< port << " ["<<service<<"] este DESCHIS!";
                if(!banner.empty() && banner!="No banner returned"){
                    std::cout<<" | Banner: "<<banner;
                } 
                std::cout<< "\n";
            }
        }));
    }
    for(auto& f : futures){
        f.wait();
    }
}

std::vector<PortResult> PortScanner::get_open_ports() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_open_ports;
}

