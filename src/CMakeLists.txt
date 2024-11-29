aux_source_directory(. SRC_LIST)
add_library(mprpc  ${SRC_LIST})

target_link_libraries(mprpc protobuf muduo_net muduo_base pthread zookeeper_mt)