# 设置源文件列表
set(SRC_LIST userclient.cpp  ../user.pb.cc)

# 定义可执行文件
add_executable(client ${SRC_LIST})

# 链接库
target_link_libraries(client mprpc protobuf)
