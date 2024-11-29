#include "mprpcconfig.h"
#include "fstream"
#include <iostream>
void MprpcConfig::LoadConfigFile(const char* config_file)
{
    std::ifstream configFile(config_file);
    std::string line;

    if (!configFile.is_open()) {
        std::cerr << "Unable to open config file!" << std::endl;
        return ;
    }

    while (std::getline(configFile, line)) 
    {
        // 忽略注释行和空行
        if (line.empty() || line[0] == '#')
            continue;

        // 找到分隔符 '=' 的位置
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) 
        {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            // 去除多余的空白符
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            _configMap[key] = value;
        }
    }
    // for(auto it = _configMap.begin();it != _configMap.end();++it)
    // {
    //     std::cout<<"key:"<<it->first<<"  value:"<<it->second<<"\n";
    // }
} 

std::string MprpcConfig::Load(std::string config_name)
{
    auto it = _configMap.find(config_name);
    if(it != _configMap.end())
    {
        return it->second;
    }

    return nullptr;
}