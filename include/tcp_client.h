#pragma once

#include "spdlog/spdlog.h"
#include <arpa/inet.h>
#include <string>
#include <future>

#include "tcp_socket.h"

namespace tcp {


class TcpClient {
    static int retry_interval; // ms
private:
    int fd_;
    std::string server_ip_;
    uint16_t server_port_;
    std::shared_ptr<TcpSocket> socket_ptr_;
    std::atomic<bool> is_connected_;
    std::mutex mu_;
    std::condition_variable cv_;

public:
    TcpClient();
    virtual ~TcpClient();
    /// retry: 连接不成功时的重试次数, 0表示无限尝试连接
    /// async: 是否非阻塞
    bool ConnectTo(const std::string &ip, uint16_t port, int retry=1, bool async=false);

    bool SendMsg(const std::string msg, int timeout=1000);
    bool RecvMsg(std::string *msg, int timeout=1000);

    bool IsConnected() const;

    void Close();

};


}