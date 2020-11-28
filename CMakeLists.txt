cmake_minimum_required(VERSION 3.15)
project(HE_kv_store_client)

set(CMAKE_CXX_STANDARD 17)
include_directories(/usr/local/include)

find_package(rpclib REQUIRED)
find_package(SEAL 3.6 REQUIRED)
add_executable(HE_kv_store_client main.cpp hash.hpp)
target_link_libraries(HE_kv_store_client rpclib::rpc)
target_link_libraries(HE_kv_store_client SEAL::seal)