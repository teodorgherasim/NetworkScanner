#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <atomic>

struct PortResult{
    int port;
    std :: string banner;
};

class PortScanner{
    public:
        PortScanner(std :: string target_ip, int start_port, int end_port, int timeout_ms = 500);
        void scan(size_t thread_count = 50);

        std :: vector<PortResult> get_open_ports() const;

    private:

        std :: string m_target_ip;
        int m_start_port;
        int m_end_port;
        int m_timeout_ms;

        std :: atomic<int> m_current_port;
        std :: vector<PortResult> m_open_ports;
        mutable std :: mutex m_mutex; //protejeaza m_open_ports si std :: cout de race conditions 

        void worker_thread();
};