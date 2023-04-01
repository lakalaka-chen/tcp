
#include "tcp_socket.h"

namespace tcp {

int TcpSocket::CreateSocket()  {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int TcpSocket::Read(std::string *msg) {
    int len = 0;
    int n_read = read_((char*)&len, 4); // 先读取消息长度
    if (n_read < 0) {
        return -1;
    }
    len = ntohl(len); // 字节序转换
    char *buf = new char[len+1];
    n_read = read_(buf, len);
    if (n_read != len) {
        delete[] buf;
        return -1;
    }
    buf[len] = '\0';
    msg->resize(len);
    for (int i = 0; i < len; i ++) {
        (*msg)[i] = buf[i];
    }
    delete[] buf;
    return len;
}

int TcpSocket::Write(const std::string &msg) {
    char *data = new char[msg.size() + 4];
    uint32_t msg_size = htonl(msg.size());
    std::memcpy(data, &msg_size, 4);
    std::memcpy(data+4, msg.data(), msg.size());

    int data_size = msg.size() + 4;
    int n_write = write_(data, data_size);
    delete[] data;
    return n_write;
}




TcpSocket::TcpSocket(int fd): socket_fd_(fd) {}
TcpSocket::~TcpSocket() {}

void TcpSocket::Close() { close(socket_fd_); }


int TcpSocket::write_(const char* data, int data_size) {
    int left = data_size;
    int n_write = 0;
    const char *p = data;
    while (left) {
        n_write = write(socket_fd_, data, left);
        if (n_write < 0) {
            return -1;
        }
        p += n_write;
        left -= n_write;
    }
    return data_size;
}

int TcpSocket::read_(char *buf, int to_read) {
    int n_read = 0;
    int left = to_read;
    char *p = buf;
    while (left) {
        n_read = read(socket_fd_, p, left);
        if (n_read < 0) {
            return -1;
        }
        p += n_read;
        left -= n_read;
    }
    return to_read;
}



}