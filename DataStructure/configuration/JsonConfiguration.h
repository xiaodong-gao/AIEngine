#ifndef JSONCONFIGURATION_H
#define JSONCONFIGURATION_H

#include "JsonBase.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")// 该指令仅支持VS环境
#endif

class JsonConfiguration : public JsonBase{
public:
    explicit JsonConfiguration(const std::string &name);
    ~JsonConfiguration();
    int parse_json_file(const std::string &json_path) override;
private:
    int camera_count_;
    int communication_count_;
    std::vector<CAMERA_FIELD> vec_camera_;
    std::vector<COMMUNICATION_FIELD> vec_communication_;
};

#endif // JSONCONFIGURATION_H
