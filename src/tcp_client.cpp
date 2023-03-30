
#include "tcp_client.h"

namespace tcp {

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