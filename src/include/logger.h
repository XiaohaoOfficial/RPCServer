#pragma once
#include "lockqueue.h"
#include <string>
enum  LogLevel
{
    INFO,
    DEBUG
};

class Logger
{
public:
    static Logger& getInstance();
    void Log(std::string msg,LogLevel level);
    
private:
    LogLevel _level;
    LockQueue<std::pair<std::string,LogLevel>> _lkQueue;
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger &&) = delete;
};