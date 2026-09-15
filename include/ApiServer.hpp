#pragma once

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include "PortScanner.hpp"

class ApiServer{
    public:
        explicit ApiServer(int port = 8080);
        ~ApiServer();

        void start();
    
    private:
        int m_port;
        std::atomic<bool> m_is_scanning{false};
        std::string m_last_target;
        std::vector<PortResult> m_last_results;
        mutable std :: mutex m_mutex;

        void setup_routes();
        void run_scan_async(std::string target, int start_port, int end_port, int timeout_ms, int threads);

        //Serviciul HTTP
        struct Impl;
        std::unique_ptr<Impl> m_impl;


};
