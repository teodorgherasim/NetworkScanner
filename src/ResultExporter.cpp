#include "ResultExporter.hpp"
#include <fstream>
#include <sstream>

static std::string escape_json_string(const std::string& input){
    std ::ostringstream ss;
    for(char c : input)
    {
        switch (c)
        {
            case '"': ss << "\\\"";break;
            case '\\': ss << "\\\\";break;
            case '\b': ss << "\\b";break;
            case '\f': ss << "\\f";break;
            case '\n': ss << "\\n";break;
            case '\r': ss << "\\r";break;
            case '\t': ss << "\\t";break;
            
            default:
            if(c>= 0 && c <=0x1f){
                //Ignoram caracterele de control 
            }else{
                ss<<c;
            }
            break;
        }
    }
    return ss.str();
}

bool ResultExporter :: to_json(const std::string& filepath,const std:: string& target_ip, const std :: vector<PortResult>& results)
{
    std :: ofstream file(filepath);
    if(!file.is_open()) return false;

    file << "{\n";
    file << "\"target\": \"" << escape_json_string(target_ip) << "\",\n";
    file << "  \"open_ports_count\": " << results.size() << ",\n";
    file << "  \"ports\": [\n";

    for(size_t i = 0; i< results.size(); ++i ){
        file << "    {\n";
        file << "      \"port\": " << results[i].port << ",\n";
        file << "      \"service\": \"" << escape_json_string(results[i].service_name) << "\",\n";
        file << "      \"banner\": \"" << escape_json_string(results[i].banner) << "\"\n";
        file << "    }" << (i + 1 < results.size() ? "," : "") << "\n";
    }

    file << "]\n";
    file << "}\n";

    return true;
}

bool ResultExporter :: to_csv(const std:: string& filepath, const std:: vector<PortResult>& results){
    std::ofstream file(filepath);
    if(!file.is_open()) return false;
    

    file << "Port,Service,Status,Banner\n";
    for(const auto& res: results){
        std:: string  clean_banner = res.banner;
        for(char &c : clean_banner){
            if(c == '"')
                c = '\'';
        }
        file << res.port << ",\"" << res.service_name << "\",OPEN,\""<<clean_banner<<"\"\n";

    }
    return true;
}