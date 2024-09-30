#include<iostream>
#include<string>
#include<memory>
#include"boost/asio.hpp"
using namespace std;
using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

class Client
{
public:
    Client(io_context& ioc, std::string serverIP, int serverPort):_socket(ioc),_serverEndPoint(make_address(serverIP), serverPort){}
    ~Client(){}

    bool Connect(){
        boost::system::error_code ec;
        _socket.connect(_serverEndPoint, ec);
        if( ec ){
            std::cout << "connect to server failed! errprMs: " << ec.message() << std::endl;
            return false;
        }
        return true;
    }

    bool SendMsg(std::string& msg){
        boost::system::error_code ec;
        // boost::asio::socket_base::broadcast option(true);
        // _socket.set_option(option, ec);
        size_t sendLen = _socket.send(boost::asio::buffer(msg), socket_base::message_do_not_route, ec);
        if( ec ){
            std::cout << "send msg failed! errorMs: " << ec.message() << std::endl;
            return false;
        }
        std::cout << "send msg success! sendLen: " << sendLen << std::endl;
        return true;
    }

    int localPort() const { return _socket.local_endpoint().port();}

private:
    tcp::socket _socket;
    tcp::endpoint _serverEndPoint;
};


int main(int argc, char** argv)
{
    std::string servIp(argv[1]);
    int servPort = std::stoi(argv[2]);
    io_context ioc;
    std::shared_ptr<Client> client = std::make_shared<Client>(ioc, servIp, servPort);
    bool ret = client->Connect();
    if( ret ){
        std::cout << "connect to server success! local port: " << client->localPort() << std::endl;
    }

    std::string msg;
    while(msg != "q"){
        getline(cin, msg);
        bool ret = client->SendMsg(msg);
        if( !ret ){
            std::cout <<" send error " << std::endl;
        }
    }
}