#include "mprpcapplication.h"
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>

MprpcConfig MprpcApplication::_config;
MprpcApplication& MprpcApplication::getInstance()
{
    static MprpcApplication app;
    return app;
}

void MprpcApplication::Init(int argc,char **argv)
{
    if(argc < 2)
    {
        showArgsHelp();
        exit(EXIT_FAILURE);
    }

    int opt;
    std::string config_file;
    while((opt = getopt(argc,argv,"i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            config_file = optarg;
            break;
        case ':':
            std::cout<<"need <configfile>\n";
            break;
        default:
            showArgsHelp();
            break;
        }
    }

    _config.LoadConfigFile("/home/yanghao/Code/mprpc/bin/test.conf");
}

MprpcApplication::MprpcApplication()
{
    
}
void MprpcApplication::showArgsHelp()
{
    std::cout<<"format:command -i<configfile>\n";
}
MprpcConfig MprpcApplication::getConfig()const 
{
    return _config;
}