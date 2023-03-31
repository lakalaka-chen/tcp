#pragma once
#include "tcp_socket.h"


namespace tcp {


using HandlerFunc = std::function<void(const std::string &, std::string &)>;
const HandlerFunc default_func = [](const std::string &recv, std::string &reply){ reply = recv; };

class TcpServer;

class TcpHandler {
private:
    HandlerFunc func_;
    std::shared_ptr<TcpSocket> socket_;     // 和客户端通信
    std::string client_ip_;
    uint16_t client_port_;


public:
    explicit TcpHandler(
            std::shared_ptr<TcpSocket> socket,
            const char *ip, uint16_t port,
            HandlerFunc func)
            : socket_(socket), client_ip_(ip), client_port_(port), func_(func) {

    }

    virtual ~TcpHandler() {}
    void StartWorking();

};

}