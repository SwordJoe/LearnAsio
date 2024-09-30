#include<iostream>
#include<string>
#include<thread>
#include<memory>
#include<functional>
#include"boost/asio.hpp"
using namespace boost::asio;
using namespace boost::system;

#define BLOCK_SIZE 128

class TcpSession
{
public:
    TcpSession(io_context& ioc, ip::tcp::socket& socket):_ioc(ioc), _socket(std::move(socket)){}
    ~TcpSession(){}

    void start(){
        while(true){
            std::string msg(100,'\0');
            error_code ec;
            size_t recvLen = 0;
            // recvLen = _socket.receive(buffer(msg, 100),0,ec);
            recvLen = _socket.read
            // while( recvLen == 0){
            //     std::cout << "[" << _socket.remote_endpoint().address().to_string() << ":" << _socket.remote_endpoint().port() << "]->["
            //                 <<_socket.local_endpoint().address().to_string() << ":" << _socket.local_endpoint().port() << "]"
            //                 << " Waiting Msg..." << std::endl;
            //     std::this_thread::sleep_for(std::chrono::seconds(10));
            // }
            if( ec ){
                std::cout << "client receive error! errorMsg: " << ec.message() << std::endl;
                break;
            }
            std::cout << msg << std::endl;
            size_t sendLen = _socket.send(buffer(msg, msg.length()), 0, ec);
            if( sendLen != recvLen ){
                std::cout << "send bytes != recv bytes. recv: " << recvLen << ", send: " << sendLen << std::endl; 
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
    ~Server(){
        for(auto t : _threads){
            if(t->joinable()){
                t->join();
            }
        }
    }

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

        _acceptor.set_option(ip::tcp::acceptor::reuse_address(true), ec);

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
            std::shared_ptr<TcpSession> client = std::make_shared<TcpSession>(_ioc, socket);
            // std::shared_ptr<std::thread> p_thread = std::make_shared<std::thread>([client](){client->start();});
            std::shared_ptr<std::thread> p_thread = std::make_shared<std::thread>(std::bind(&TcpSession::start, client.get()));
            _threads.push_back(p_thread);
            _clients.push_back(client);
        }
    }

private:
    io_context& _ioc;
    std::string _ipAddr;
    int _ipPort;
    ip::tcp::acceptor _acceptor;
    std::vector<std::shared_ptr<TcpSession>> _clients;
    std::vector<std::shared_ptr<std::thread>> _threads;
};




int main(int argc, char** argv)
{
    std::shared_ptr<io_context> ioc = std::make_shared<io_context>();
    std::shared_ptr<Server> serv = std::make_shared<Server>(*ioc, std::string(argv[1]), std::stoi(argv[2]));
    bool ret = serv->start();
    if( !ret ){
        return -1;
    }

    serv->loopAccept();
    ioc->run();
}