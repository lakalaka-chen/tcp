
#include "tcp_client.h"

namespace tcp {

int TcpClient::retry_interval = 200;

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

bool TcpClient::ConnectTo(const std::string &ip, uint16_t port, int retry, bool async) {

    assert(retry >= 0);

    Close();
    server_port_ = port;
    server_ip_ = ip;


    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip.data(), &address.sin_addr.s_addr);

    int code = -1;
    if (retry == 0) {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(TcpClient::retry_interval));
            fd_ = TcpSocket::CreateSocket();
            code = connect(fd_, (struct sockaddr*)&address, sizeof(address));
            spdlog::info("Connecting  [{}:{}] ... ", ip, port);
            if (code < 0) {
                close(fd_);
                continue;
            } else {
                break;
            }
        }
    } else {
        while (retry--) {
            std::this_thread::sleep_for(std::chrono::milliseconds(TcpClient::retry_interval));
            fd_ = TcpSocket::CreateSocket();
            code = connect(fd_, (struct sockaddr*)&address, sizeof(address));
            spdlog::info("Connecting  [{}:{}] ... ", ip, port);
            if (code < 0) {
                close(fd_);
            } else {
                break;
            }
        }
    }

    if (code >= 0) {
        spdlog::info("Connect Success! ");
        socket_ptr_ = std::make_shared<TcpSocket>(fd_);
        return true;
    } else {
        spdlog::error("Connect Failed! ");
        close(fd_);
        return false;
    }

//    code = connect(fd_, (struct sockaddr*)&address, sizeof(address));
//    if (code < 0) {
//        spdlog::error("Failed to connect. ");
//        close(fd_);
//        return false;
//    }
//    spdlog::debug("Connect to {}:{}", ip, port);
//    socket_ptr_ = std::make_shared<TcpSocket>(fd_);
//    return true;
}

}