#pragma once
#include <unordered_map>
#include <string>

/*
rpcserverip rpcserverport
zookeeperip zookeeperport
*/
class MprpcConfig
{
public:
    //加载配置文件  
    void LoadConfigFile(const char *config_file);
    //查询配置项信息
    std::string Load(std::string config_name);

private:
    std::unordered_map<std::string,std::string> _configMap;
};