#include <iostream>
#include <unistd.h>
#include <thread>
using namespace std;

int main() 
{
    FILE *file = fopen("test.txt","a+");
    fputs("nihao\n",file);
    fclose(file);
    time_t now= time(nullptr);
                                std::cout<<5555<<std::endl;

                tm *nowtm = localtime(&now);
                
                cout<<nowtm->tm_mday<<endl;
    return 0;
}