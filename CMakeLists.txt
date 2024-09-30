cmake_minimum_required(VERSION 3.10)
project(asio_test)

add_executable(client src/client.cc)
add_executable(server src/server.cc)


find_package(Boost REQUIRED)

target_link_libraries(client PRIVATE boost::boost)
target_link_libraries(server PRIVATE boost::boost)