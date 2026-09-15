#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#include "httplib.h"
#include "ApiServer.hpp"
#include "ResultExporter.hpp"
#include <sstream>
#include <iostream>

struct ApiServer :: Impl {
    httplib::Server svr;
};

ApiServer::ApiServer(int port)
    : m_port(port) , m_impl(std::make_unique<Impl>()){}


ApiServer::~ApiServer() =default;

void ApiServer::setup_routes() {

    m_impl->svr.set_mount_point("/","./public");
    // 1. GET /api/v1/status - Verifica daca exista o scanare in desfasurare
    m_impl->svr.Get("/api/v1/status", [this](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string status = m_is_scanning ? "scanning" : "idle";
        
        std::ostringstream json;
        json << "{\"status\":\"" << status << "\",\"last_target\":\"" << m_last_target << "\"}";
        
        res.set_content(json.str(), "application/json");
    });

    // 2. GET /api/v1/results - Returneaza ultimele rezultate
    m_impl->svr.Get("/api/v1/results", [this](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::ostringstream json;
        json << "{\n  \"target\": \"" << m_last_target << "\",\n";
        json << "  \"count\": " << m_last_results.size() << ",\n";
        json << "  \"ports\": [\n";

        for (size_t i = 0; i < m_last_results.size(); ++i) {
            json << "    {\"port\": " << m_last_results[i].port 
                 << ", \"service\": \"" << m_last_results[i].service_name 
                 << "\", \"banner\": \"" << m_last_results[i].banner << "\"}"
                 << (i + 1 < m_last_results.size() ? "," : "") << "\n";
        }
        json << "  ]\n}";

        res.set_content(json.str(), "application/json");
    });

    // 3. POST /api/v1/scan - Porneste o scanare noua
    // Params via query/body: target, start, end, threads, timeout
    m_impl->svr.Post("/api/v1/scan", [this](const httplib::Request& req, httplib::Response& res) {
        if (m_is_scanning) {
            res.status = 409; // Conflict
            res.set_content("{\"error\":\"A scan is already in progress\"}", "application/json");
            return;
        }

        std::string target = req.has_param("target") ? req.get_param_value("target") : "127.0.0.1";
        int start_port = req.has_param("start") ? std::stoi(req.get_param_value("start")) : 1;
        int end_port = req.has_param("end") ? std::stoi(req.get_param_value("end")) : 1024;
        int threads = req.has_param("threads") ? std::stoi(req.get_param_value("threads")) : 50;
        int timeout = req.has_param("timeout") ? std::stoi(req.get_param_value("timeout")) : 500;

        std::thread([this, target, start_port, end_port, timeout, threads]() {
            run_scan_async(target, start_port, end_port, timeout, threads);
        }).detach();

        res.set_content("{\"message\":\"Scan started successfully\"}", "application/json");
    });
}

void ApiServer::run_scan_async(std::string target, int start_port, int end_port, int timeout_ms, int threads) {
    m_is_scanning = true;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_last_target = target;
        m_last_results.clear();
    }

    PortScanner scanner(target, start_port, end_port, timeout_ms);
    scanner.scan(threads);

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_last_results = scanner.get_open_ports();
    }
    m_is_scanning = false;
}

void ApiServer::start() {
    setup_routes();
    std::cout << "[+] Server REST API pornit pe http://localhost:" << m_port << "\n";
    std::cout << "[+] Endpoints disponibile:\n";
    std::cout << "    - GET  /api/v1/status\n";
    std::cout << "    - GET  /api/v1/results\n";
    std::cout << "    - POST /api/v1/scan?target=IP&start=1&end=1024\n";
    
    m_impl->svr.listen("0.0.0.0", m_port);
}


