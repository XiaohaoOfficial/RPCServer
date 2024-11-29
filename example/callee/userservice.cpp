#include "user.pb.h"
#include <string>
#include <iostream>
#include "mprpcapplication.h"
#include "rpcprovider.h"
using namespace std;
class UserService : public mata::UserServiceRPC
{
public:
    bool Login(string name, string pwd)
    {
        cout<<"doing login()\n";
        cout<<"name :"<<name<<"  password :"<<pwd<<endl;
        return true;
    }

    /*
    在callee中重写RPC方法分为以下三步：
    1.读取request中的参数。
    2.将request中的参数传入需要调用的函数中，运行该函数，并将运行结果返回，传入response中。
    3.调用done->run()方法。
    */
    void login(google::protobuf::RpcController* controller,
                       const ::mata::LoginRequest* request,
                       ::mata::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //1.读取request中的参数。
        string name = request->name();
        string password = request->password();
        response->set_success(Login(name,password));

        //2.将request中的参数传入需要调用的函数中，运行该函数，并将运行结果返回，传入response中。
        auto result  = response->mutable_result();
        result->set_err(0);
        result->set_errmsg("no error");

        
        //3.调用done->run()方法。
        done->Run();
    }
};

//以下是希望借助框架，callee完成的一般性流程
int main(int argc,char** argv)
{
    //调用框架的初始化操作
    MprpcApplication::Init(argc,argv);

    RpcProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();

    return 0;   

}
