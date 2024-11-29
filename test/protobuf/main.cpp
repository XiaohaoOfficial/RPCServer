#include "test.pb.h"
#include <iostream>
#include <string>
using namespace std;
class User
{
public:
    string name;
    int age;
    enum Sex
    {
        MAN,
        WOMAN
    };
    Sex sex;
    User(string name,int age,Sex sex):name(name),sex(sex),age(age){}
};
int main() 
{
    //封装了login请求对象的数据
    mata::LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");
    string send_str = req.SerializeAsString();
    {
        cout<<send_str<<endl;
    }

    //从send_str反序列化一个login请求对象
    mata::LoginRequest reqB;
    if(reqB.ParseFromString(send_str))
    {
        cout<<reqB.name()<<endl;
        cout<<reqB.pwd()<<endl;
    }
    cout<<"---------------------\n";
    mata::ResponseUserList reqC;
    auto user1 = reqC.add_users();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(mata::User::MAN);
    
    auto user2 = reqC.add_users();
    user2->set_name("li si");
    user2->set_age(18);
    user2->set_sex(mata::User::WOMAN);
    
    string reqC_str = reqC.SerializePartialAsString();
    cout<<reqC_str<<endl;
    mata::ResponseUserList respA;
    respA.ParseFromString(reqC_str);
    int size = respA.users_size();
    cout<<size<<endl;
    for(int i = 0;i<size;++i)
    {
        auto it = respA.users(i);
        cout<<it.name()<<"\t"<<it.age()<<"\t"<<it.sex()<<endl;
    }


    return 0;
}