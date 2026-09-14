#pragma once

#include <string>
#include <vector>
#include "PortScanner.hpp"

class ResultExporter{
    public:
        static bool to_json(const std::string& filepath,const std:: string& target_ip, const std :: vector<PortResult>& results);
        static bool to_csv(const std:: string& filepath, const std:: vector<PortResult>& results);
        
};