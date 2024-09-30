#include<iostream>
#include<string>
#include"boost/asio.hpp"
using namespace boost::asio;
using namespace boost::system;

#define BLOCK_SIZE 128

class TcpClient
{
public:
    TcpClient(io_context& ioc, ip::tcp::socket& socket):_ioc(ioc), _socket(std::move(socket)){}
    ~TcpClient(){}

    void start(){
        while(true){
            std::string msg;
            error_code ec;
            size_t recvLen = _socket.receive(buffer(msg, 100), 0, ec);
            while( recvLen == 0){
                std::cout << "wait msg..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            if( ec ){
                std::cout << "client receive error! errorMsg: " << ec.message() << std::endl;
                break;
            }
            std::cout << msg << std::endl;
            size_t sendLen = _socket.send(buffer(msg), 0, ec);
            if( sendLen != recvLen ){
                std::cout << "send bytes != recv bytes" << std::endl;
                break;
            }
            if( ec ){
                std::cout << "client write error! errorMsg: " << ec.message() << std::endl;
                break;
            }
        }
    }
private:
    io_context& _ioc;
    ip::tcp::socket _socket;
};


class Server
{
public:
    Server(io_context& ioc, std::string ipAddr, int ipPort): _ioc(ioc), _ipAddr(ipAddr), _ipPort(ipPort), _acceptor(ioc){}
    ~Server(){}

    bool start(){
        error_code ec;
        ip::tcp::endpoint ep(ip::make_address(_ipAddr, ec), _ipPort);
        if( ec ){
            std::cout << "ip address error!" << " Error Code = " << ec.value() << " Error Message: " << ec.message() << std::endl;
            return false;
        }
        ec.clear();

        _acceptor.open(ep.protocol(), ec);
        if( ec ){
            std::cout << "acceptor open error!" << " Error Code = " << ec.value() << " Error Message: " << ec.message() << std::endl;
            return false;
        }
        ec.clear();

        _acceptor.bind(ep, ec);
        if( ec ){
            std::cout << "acceptor bind error!" << " Error Code = " << ec.value() << " Error Message: " << ec.message() << std::endl;
            return false;
        }
        ec.clear();

        _acceptor.listen(BLOCK_SIZE, ec);
        if( ec ){
            std::cout << "acceptor listen error!" << " Error Code = " << ec.value() << " Error Message: " << ec.message() << std::endl;
            return false;
        }
        ec.clear();

        return true;
    }
    std::string ipAddr() const { return _acceptor.local_endpoint().address().to_string(); }
    int port() const { return _acceptor.local_endpoint().port();}
    void loopAccept(){
        
        error_code ec;
        while(true){
            ip::tcp::socket socket(_ioc);
            ec.clear();
            _acceptor.accept(socket, ec);
            if( ec ){
                std::cout << "acceptor accept error!" << " Error Code = " << ec.value() << " Error Message: " << ec.message() << std::endl;
                continue;
            }
            std::cout << "new client connected! [" << socket.remote_endpoint().address().to_string() << ":" << socket.remote_endpoint().port() << "]" << std::endl;
            std::shared_ptr<TcpClient> client = std::make_shared<TcpClient>(_ioc, socket);
            client->start();
            _clients.push_back(client);
        }
    }

private:
    io_context& _ioc;
    std::string _ipAddr;
    int _ipPort;
    ip::tcp::acceptor _acceptor;
    std::vector<std::shared_ptr<TcpClient>> _clients;
};




int main()
{
    std::shared_ptr<io_context> ioc = std::make_shared<io_context>();
    std::shared_ptr<Server> serv = std::make_shared<Server>(*ioc, "0.0.0.0", 8891);
    bool ret = serv->start();
    if( !ret ){
        return -1;
    }

    serv->loopAccept();
    ioc->run();
}