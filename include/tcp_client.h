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
    TcpClient(): socket_ptr_(nullptr) {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
    };

    virtual ~TcpClient() {
        Close();
    }

    bool ConnectTo(const std::string &ip, uint16_t port);

    bool SendMsg(const std::string msg) {
        assert(socket_ptr_ != nullptr);
        int n_send = socket_ptr_->Write(msg);
        return n_send >= 0;
    }

    bool RecvMsg(std::string *msg) {
        assert(socket_ptr_ != nullptr);
        int n_recv = socket_ptr_->Read(msg);
        return n_recv >= 0;
    }


    void Close() {
        if (socket_ptr_ != nullptr) {
            socket_ptr_.reset();
        }
    }

};


}