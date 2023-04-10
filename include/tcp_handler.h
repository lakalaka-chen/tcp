#pragma once
#include "tcp_socket.h"

#include <atomic>

namespace tcp {


using HandlerFunc = std::function<void(const std::string &, std::string &)>;
const HandlerFunc default_func = [](const std::string &recv, std::string &reply){ reply = recv; };

class TcpServer;

class TcpHandler {
    friend class TcpServer;
private:
    HandlerFunc func_;
    std::shared_ptr<TcpSocket> socket_;     // 和客户端通信
    std::string client_ip_;
    uint16_t client_port_;
    std::string server_name_;
    std::atomic<bool> is_running_;


public:
    explicit TcpHandler(
            std::shared_ptr<TcpSocket> socket,
            const char *ip, uint16_t port,
            HandlerFunc func)
            : socket_(socket), client_ip_(ip), client_port_(port), func_(func), is_running_(true) {

    }

    virtual ~TcpHandler() {}
    void StartWorking();
    void Disconnect() {
        is_running_.store(false);
    }

};

}