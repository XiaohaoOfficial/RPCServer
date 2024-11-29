# 设置源文件列表
set(SRC_LIST userservice.cpp  ../user.pb.cc)

# 定义可执行文件
add_executable(provider ${SRC_LIST})

# 链接库
target_link_libraries(provider mprpc protobuf)
