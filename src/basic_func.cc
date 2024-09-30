#include <iostream>
#include <string>
#include "boost/asio.hpp"
using namespace boost;
using namespace boost::asio;

//客户端创建终端节点
int create_client_endpoint(){
    std::string servIpAddr = "114.132.243.253";
    unsigned short servPort = 1234;
    // 创建一个客户端的TCP endpoint
    asio::ip::tcp::endpoint servep(ip::address::from_string(servIpAddr), servPort);
    std::cout << servep.address().to_string() << std::endl;
    std::cout << servep.port() << std::endl;
    return 0;
}

//服务端创建终端节点
int create_serv_endpoint(){
    //注意这里的地址类型是asio里的adress类型，不是xxx.xxx.xxx.xxx的点分十进制的std::string类型
    ip::address servIpAddr = ip::address_v4::any();
    ip::tcp::endpoint endpoint(servIpAddr, 1234);
}

//创建普通tcp socket，
int create_tcp_socket(){
    //1.创建io_context
    io_context ioc;

    //2. 选择协议
    ip::tcp protocol = ip::tcp::v4();

    //3. 创建socket，并指定协议
    // ip::tcp::socket sock(ioc);
    //ip::tcp::socket sock(ioc, ip::tcp::v4());
    ip::tcp::socket sock(ioc, ip::tcp::endpoint(ip::address::from_string("192.168.15.193"), 9999));

    std::cout << sock.local_endpoint().address().to_string() << ":" << sock.local_endpoint().port() << std::endl;
    //4. 打开socket
    //system::error_code ec;
    //sock.open(protocol, ec);

    // if(ec.value() != 0 ){
    //     std::cout << "tcp socket open failed" << std::endl;
    //     std::cout << "error code: " << ec.value() << ", error message: " << ec.message() << std::endl;
    //     return -1;
    // }
    return 0;
}

//服务器创建acceptor socket，并绑定端口
int create_acceptor_socket_and_bind(){
    system::error_code ec;

    //1. 创建io_context
    io_context ioc;

    //2. 选择协议
    ip::tcp protocol = ip::tcp::v4();

    //3. 创建acceptor socket，同时绑定一个终端节点(端口)
    ip::tcp::acceptor acceptor(ioc, ip::tcp::endpoint(ip::address_v4::any(), 1234));
    
    // //4. 绑定端口
    // acceptor.bind(ip::tcp::endpoint(ip::address_v4::any(), 1234), ec);
    // if(ec.value() != 0){
    //     std::cout << "acceptor bind failed!" << std::endl;
    //     std::cout << "error code: " << ec.value() << ", error message: " << ec.message() << std::endl;
    //     return -1;
    // }
    return 0;
}

//客户端连接到服务器端口
int client_connect_to_server_endpoint(){
    std::string servAddr("192.168.15.192");
    unsigned short servPort = 4545;
    ip::tcp::endpoint servep(ip::address::from_string(servAddr), servPort);
    io_context ioc;
    ip::tcp::socket sock(ioc, ip::tcp::v4());
    try{
        sock.connect(servep);
    }
    catch(system::system_error& e){
        std::cout << "Error occured! Error Code: " << e.code() << " Message: " << e.what() << std::endl;
        return e.code().value();
    }
    return 0;
}

//服务器端接收客户端连接
int server_accept_client_connection(){
    const int BLOCK_SIZE = 30;
    unsigned short port = 3333;
    ip::tcp::endpoint ep(ip::address_v4::any(), port);
    system::error_code ec;

    io_context ioc;
    ip::tcp::acceptor accept(ioc, ip::tcp::v4());
    accept.bind(ep);
    accept.listen(BLOCK_SIZE);

    ip::tcp::socket sock(ioc);
    accept.accept(sock, ec);
    if(ec.value() != 0){
        std::cout << "accept failed!" << std::endl;
        std::cout << "error code: " << ec.value() << ", error message: " << ec.message() << std::endl;
        return -1;
    }

}
