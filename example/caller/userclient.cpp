#include "user.pb.h"
#include <iostream>
#include "mprpcapplication.h"

using namespace std;

int main(int argc,char **argv) 
{

    MprpcApplication::Init(argc,argv);

    auto usrs = mata::UserServiceRPC_Stub (new MprpcChannel());

    mata::LoginRequest request;
    request.set_name("zhang san");
    request.set_password("1113333");

    mata::LoginResponse response;
    MprpcController controller;
    usrs.login(&controller,&request,&response,nullptr);
    if(controller.Failed())
    {
        std::cerr<<controller.ErrorText()<<std::endl;
        return 0;
    }
    //Logger::getInstance().Log("记录一下Log",INFO);
    cout<<"-----------------\n";
    cout<<response.success()<<endl;
    
    return 0;
}