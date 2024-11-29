#include "mprpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <google/protobuf/descriptor.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mprpcapplication.h>
#include <netinet/in.h>
#include <unistd.h>
 /*
        收到消息的组成结构为：RPC HeadersSize（int32)+RPC Header+args
        RPCheader 包括 serviceName methodName
*/
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    std::string method_name = method->name();
    auto sd = method->service();
    std::string service_name = sd->name();
    uint32_t args_size  =0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        std::cerr<<"args serialize error!\n";
        controller->SetFailed("request->SerializeToString(&args_str) error");
        return;
    }
    header::RpcHeader req;
    req.set_argssize(args_size);
    req.set_methodname(method_name);
    req.set_servicename(service_name); 

    std::string header_str;
    req.SerializeToString(&header_str);

    int32_t header_size = header_str.size();
    std::string send_str;
    send_str.insert(0, std::string(reinterpret_cast<char*>(&header_size), sizeof(int32_t)));
    send_str = send_str + header_str +  args_str;


    int clientFd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == clientFd)
    {
        std::cerr<<"create socket error! errno:"<<errno<<std::endl;
        close(clientFd);
        exit(EXIT_FAILURE);
    }
    // 读取配置文件rpcserver的信息
    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    // rpc调用方想调用service_name的method_name服务，需要查询zk上该服务所在的host信息
    ZkClient zkCli;
    zkCli.Start();
    //  /UserServiceRpc/Login
    std::string method_path = "/" + service_name + "/" + method_name;
    // 127.0.0.1:8000
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "")
    {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }

    struct sockaddr_in server_addr;
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str()); 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port= (htons(port));
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(-1 == connect(clientFd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        std::cerr<<"connect error!\n";
        close(clientFd);
        controller->SetFailed("connect(clientFd,(struct sockaddr*)&server_addr,sizeof(server_addr) error");
        exit(EXIT_FAILURE);
    }
    if(-1 == send(clientFd,send_str.c_str(),send_str.size(),0))
    {
        std::cerr<<"send error!\n";
        close(clientFd);
        controller->SetFailed("send(clientFd,send_str.c_str(),send_str.size(),0) error");

        exit(EXIT_FAILURE);
    }
    char recv_buffer[1024]={0};
    int recv_size = 0;
    if(-1 ==(recv_size = recv(clientFd,recv_buffer,1024,0)))
    {
         std::cerr<<"receive error!\n";
         close(clientFd);
         controller->SetFailed("recv(clientFd,recv_buffer,1024,0) error");

         return;
         
    }

    if(!response->ParseFromArray(recv_buffer,recv_size))
    {
        std::cerr<<"parse error!\n";
        close(clientFd);
        controller->SetFailed("ParseFromArray(recv_buffer,recv_size) error");
        exit(EXIT_FAILURE);
    }
    close(clientFd);
    std::cout<<"method call finish\n";
}