#pragma once

#include "spdlog/spdlog.h"
#include <atomic>
#include <memory>
#include <cstring>
#include <thread>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <functional>

#include "tcp_socket.h"

namespace tcp {

using HandlerFunc = std::function<void(const std::string &, std::string &)>;
const HandlerFunc default_func = [](const std::string &recv, std::string &reply){ reply = recv; };

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


class TcpServer {

protected:
    std::string name_;
    HandlerFunc func_;                  // 收到消息之后回复什么
    std::atomic<bool> is_running_;
    uint16_t port_;
    size_t capacity_;                   // 最大接收的客户端连接数量
    int server_fd_;
    std::thread main_thread_;           // 接收客户端的线程, 但是不处理

public:
    explicit TcpServer(std::string name, uint16_t port, size_t capacity=128);
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    virtual ~TcpServer();

    bool Start();
    bool IsRunning();

    void RegisterAction(HandlerFunc func);

    bool Close();

protected:
    bool init();
    void stop();
};

}