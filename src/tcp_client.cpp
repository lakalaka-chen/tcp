
#include "tcp_client.h"

namespace tcp {

TcpClient::TcpClient(): socket_ptr_(nullptr), server_port_(0) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
};

TcpClient::~TcpClient() {
    Close();
}

bool TcpClient::SendMsg(const std::string msg) {
    assert(socket_ptr_ != nullptr);
    int n_send = socket_ptr_->Write(msg);
    return n_send >= 0;
}

bool TcpClient::RecvMsg(std::string *msg) {
    assert(socket_ptr_ != nullptr);
    int n_recv = socket_ptr_->Read(msg);
    return n_recv >= 0;
}

void TcpClient::Close() {
    if (socket_ptr_ != nullptr) {
        socket_ptr_.reset();
    }
}

bool TcpClient::ConnectTo(const std::string &ip, uint16_t port) {

    Close();
    server_port_ = port;
    server_ip_ = ip;

    fd_ = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip.data(), &address.sin_addr.s_addr);
    int code = connect(fd_, (struct sockaddr*)&address, sizeof(address));
    if (code < 0) {
        spdlog::error("Failed to connect. ");
        close(fd_);
        return false;
    }
    spdlog::debug("Connect to {}:{}", ip, port);
    socket_ptr_ = std::make_shared<TcpSocket>(fd_);
    return true;
}

}