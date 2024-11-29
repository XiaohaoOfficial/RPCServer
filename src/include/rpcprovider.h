#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <functional>
#include <unordered_map>
#include <google/protobuf/descriptor.h>

//框架提供的专门发布rpc服务的网络对象类
class RpcProvider
{
public:
    //发布RPC方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    //启动RPC服务节点，开始提供rpc远程服务调用服务
    void Run();
private:
    struct serviceInfo
    {
        //存储一个服务的对象
        google::protobuf::Service *service;
        //存储一个服务内的方法，<方法名,方法描述符>
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor *> methodMap;
    };
    //存储全部服务 <服务名，服务信息>
    std::unordered_map<std::string,serviceInfo> _serviceMap;
    muduo::net::EventLoop *m_loop;
    std::unique_ptr<muduo::net::TcpServer> m_tcpserverPtr;
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    void onMessage(const muduo::net::TcpConnectionPtr &conn,muduo::net::Buffer *,muduo::Timestamp);
    void sendRpcResponse(const muduo::net::TcpConnectionPtr &conn,google::protobuf::Message*);
};