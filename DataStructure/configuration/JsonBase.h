#ifndef JSONBASE_H
#define JSONBASE_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")// 该指令仅支持VS环境
#endif

typedef struct CAMERA_FIELD{
    std::string camera_type_{""};
    int camera_exposure_time_{0};
    std::string camera_trigger_{"off"};
    std::string camera_configuration_file_{""};
}CAMERA_FIELD;

typedef struct COMMUNICATION_FIELD{
    std::string communication_type_{""};
    std::string communication_ip_{""};
    int communication_port_{0};

}COMMUNICATION_FIELD;

class JsonBase{
public:
    explicit JsonBase(const std::string &name);
    ~JsonBase();
    virtual int parse_json_file(const std::string &json_path) = 0;
private:
    std::string name_;
};

#endif // JSONBASE_H
