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
#include "tcp_handler.h"

namespace tcp {


class TcpServer: public std::enable_shared_from_this<TcpServer> {

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

    virtual bool Start();
    bool IsRunning();

    void HandleReceiveData(HandlerFunc func);

    bool Close();

protected:
    bool init();
    void stop();
};

}