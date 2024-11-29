#include "logger.h"
#include <thread>
#include <chrono>
#include <iomanip>  // 用于 std::put_time
#include <ctime>    // 用于 std::tm
#include <cstdio>
#include <iostream>
#include <unistd.h>
Logger& Logger::getInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    std::thread write(
        [&]()->void
        {
            while(true)
            {
                std::cout<<12121<<std::endl;

                time_t now= time(nullptr);
                                std::cout<<5555<<std::endl;
                tm nowtm;
                localtime_r(&now, &nowtm);
                char file_name[80];
                sprintf(file_name, "%d-%d-%d-log.txt", nowtm.tm_year+1900, nowtm.tm_mon+1, nowtm.tm_mday);

                FILE *file = fopen(file_name,"a+");
                if(file == nullptr)
                {
                    std::cerr<<"file create error!\n";
                    exit(EXIT_FAILURE);
                }

                auto it  = _lkQueue.pop();
                char time_buf[128] = {0};
                std::string msg = it.first;
                sprintf(time_buf, "%d:%d:%d =>[%s] ", 
                    nowtm.tm_hour, 
                    nowtm.tm_min, 
                    nowtm.tm_sec,
                    (it.second == INFO ? "info" : "error"));
                msg.insert(0, time_buf);
                msg.append("\n");
                                std::cout<<333<<std::endl;

                fputs(msg.c_str(), file);
                fclose(file);
            }
        }
    );
}



void Logger::Log(std::string msg,LogLevel level)
{
                    char cwd[1024];
                getcwd(cwd, sizeof(cwd));
    _lkQueue.push({msg,level});
}