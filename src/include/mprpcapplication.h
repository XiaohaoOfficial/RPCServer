#pragma once
#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "logger.h"
#include <string>
#include "zookeeperutil.h"

class MprpcApplication
{
public:
    static void Init(int argc,char **argv);
    static MprpcApplication& getInstance();
    static void showArgsHelp();
    MprpcConfig getConfig()const;
private:
    MprpcApplication();
    MprpcApplication(const MprpcApplication&)=delete;
    MprpcApplication(MprpcApplication&&)=delete;
    static MprpcConfig _config;
    
};