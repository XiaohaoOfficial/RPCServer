#include "rpcprovider.h"
#include <memory>
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "google/protobuf/message.h"
#include <functional>
    //发布RPC方法的函数接口
    void RpcProvider::NotifyService(google::protobuf::Service *service)
    {
        const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
        std::string service_name = pserviceDesc->name();
        int method_count = pserviceDesc->method_count();
        serviceInfo serviceinfo;
        std::unordered_map<std::string,google::protobuf::MethodDescriptor *> descMap;
        serviceinfo.service = service;
        for(int i = 0;i< method_count ;++i)
        {
            
            //获取了服务对象指定下标的服务方法的描述
            auto methodDesc = pserviceDesc->method(i);
            std::string method_name = methodDesc->name();
            serviceinfo.methodMap.insert({method_name,methodDesc});
        }
        _serviceMap.insert({service_name,serviceinfo});
    }

    //启动RPC服务节点，开始提供rpc远程服务调用服务
    void RpcProvider::Run()
    {

        auto config = MprpcApplication::getInstance().getConfig();
        std::string ip = config.Load("rpcserverip");
        uint32_t port = stoi(config.Load("rpcserverport"));
        m_loop = new muduo::net::EventLoop();
        muduo::net::InetAddress addr(config.Load("rpcserverip"),stoi(config.Load("rpcserverport")));
        muduo::net::TcpServer server(m_loop,addr,"RpcProvider");
        server.setThreadNum(4);
        server.setConnectionCallback(std::bind(&RpcProvider::onConnection,this,std::placeholders::_1));
        server.setMessageCallback(std::bind(&RpcProvider::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        
        // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
        // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
        ZkClient zkCli;
        zkCli.Start();
        // service_name为永久性节点    method_name为临时性节点
        for (auto &sp : _serviceMap) 
        {
            // /service_name   /UserServiceRpc
            std::string service_path = "/" + sp.first;
            zkCli.Create(service_path.c_str(), nullptr, 0);
            for (auto &mp : sp.second.methodMap)
            {
                // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
                std::string method_path = service_path + "/" + mp.first;
                char method_path_data[128] = {0};
                sprintf(method_path_data, "%s:%d", ip.c_str(), port);
                // ZOO_EPHEMERAL表示znode是一个临时性节点
                zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
            }
        }
        
        server.start();
        m_loop->loop();
    }
    void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
    {
        if(!conn->connected())
        conn->shutdown();
    }
    void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn,muduo::net::Buffer *buffer,muduo::Timestamp)
    {
        /*
        收到消息的组成结构为：RPC HeadersSize（int32)+RPC Header+args
        RPCheader 包括 serviceName methodName
        */
        //取缓冲区内的数据
        std::string bufferStr = buffer->retrieveAllAsString();
        //取头文件的长度
        int32_t headerSize;

        bufferStr.copy(reinterpret_cast<char*>(&headerSize), sizeof(int32_t), 0);
        //bufferStr.copy((char )headerSize,4,0);

        std::string rpcHeaderStr = bufferStr.substr(4,headerSize);
        std::cout<<headerSize<<std::endl;
        header::RpcHeader rpcheader;
        rpcheader.ParseFromString(rpcHeaderStr);
        //将头文件反序列化，获得serviceName methodName 
        std::string serviceName = rpcheader.servicename();
        std::string methodName = rpcheader.methodname();
                std::cout<<"methodName:"<<methodName<<std::endl;
                std::cout<<"servicename:"<<serviceName<<std::endl;

        int32_t argsSize = rpcheader.argssize();
        auto sit = _serviceMap.find(serviceName);
        if(sit == _serviceMap.end())
        {
            std::cerr<<"servive:"<<serviceName<<" is not exist\n";
            return;
        }
        google::protobuf::Service *service = sit->second.service;
        auto mit = sit->second.methodMap.find(methodName);
        if(mit == sit->second.methodMap.end())
        {
            std::cerr<<"method"<<methodName<<"is not exist!\n";
            return;
        }
        auto method = mit->second;
        auto request = service->GetRequestPrototype(method).New();
        auto response = service->GetResponsePrototype(method).New();
        request->ParseFromString(bufferStr.substr(4+headerSize,bufferStr.size()));
        auto done = google::protobuf::NewCallback
        <RpcProvider,const muduo::net::TcpConnectionPtr &,google::protobuf::Message*>
        (this,&RpcProvider::sendRpcResponse,conn,response);
    
        service->CallMethod(method,nullptr,request,response,done);
        
    }

void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr &conn,google::protobuf::Message *response)
{
    std::string respStr;
    if(response->SerializeToString(&respStr))
    {
        conn->send(respStr);
    }
    else
    {
        std::cerr<<"serialize responseStr err!\n";
    }
    conn->shutdown();   //模拟http的短链接服务
}
