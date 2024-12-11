#include "JsonConfiguration.h"

JsonConfiguration::JsonConfiguration(const std::string &name)
    : JsonBase{name}
{

}

JsonConfiguration::~JsonConfiguration(){

}

int JsonConfiguration::parse_json_file(const std::string &json_path){

    // 打开文件
    std::ifstream inputFile(json_path);
    if (!inputFile.is_open()) {
        //std::cerr << "无法打开文件: " << json_path << std::endl;
        return -1;
    }

    // 使用std::string读取文件内容
    std::string json_str((std::istreambuf_iterator<char>(inputFile)),
                             std::istreambuf_iterator<char>());

    try {
           // 解析 JSON
           json j = json::parse(json_str);
           // 提取 camera 信息
           int camera_count = j["camera count"];
           if (camera_count > 0) {
               std::string camera_type = j["camera1"]["camera type"];
               std::string configuration_file = j["camera1"]["configuration_file"];
               int exposure_time = j["camera1"]["exposure time"];
               std::string trigger_status = j["camera1"]["trigger"];
           }

           // 提取 communication 信息
           int communication_count = j["communication count"];
           if (communication_count > 0) {
               std::string communication_type = j["communication1"]["communication type"];
               std::string ip = j["communication1"]["ip"];
               int port = j["communication1"]["port"];
               int com_port = j["communication1"]["COM"];
               int data_bits = j["communication1"]["Data Bits"];
               std::string parity_bit = j["communication1"]["Parity Bit"];
               int stop_bit = j["communication1"]["Stop Bit"];
               int baud_rate = j["communication1"]["Baud Rate"];
               std::string flow_control = j["communication1"]["Flow Control"];
               std::string data_format = j["communication1"]["Data Format"];
           }
       } catch (json::parse_error& e) {
           //std::cerr << "JSON parse error: " << e.what() << "\n";
       } catch (json::type_error& e) {
           //std::cerr << "JSON type error: " << e.what() << "\n";
       } catch (std::exception& e) {
           //std::cerr << "Error: " << e.what() << "\n";
       }
    // 关闭文件
    inputFile.close();
}
