#pragma once
#include "boost/asio.hpp"
using namespace boost;
using namespace boost::asio;

//客户端创建终端节点
int create_client_endpoint();

//服务端创建终端节点
int create_serv_endpoint();

//创建普通tcp socket，
int create_tcp_socket();

//服务器创建acceptor socket，并绑定端口
int create_acceptor_socket_and_bind();

//客户端连接到服务器端口
int client_connect_to_server_endpoint();

//服务器端接收客户端连接
int server_accept_client_connection();
