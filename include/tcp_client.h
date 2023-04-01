#pragma once

#include "spdlog/spdlog.h"
#include <arpa/inet.h>
#include <string>

#include "tcp_socket.h"

namespace tcp {


class TcpClient {
private:
    int fd_;
    std::string server_ip_;
    uint16_t server_port_;
    std::shared_ptr<TcpSocket> socket_ptr_;

public:
    TcpClient();
    virtual ~TcpClient();
    bool ConnectTo(const std::string &ip, uint16_t port);

    bool SendMsg(const std::string msg);
    bool RecvMsg(std::string *msg);

    void Close();

};


}