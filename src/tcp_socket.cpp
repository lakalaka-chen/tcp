
#include "tcp_socket.h"

namespace tcp {

int TcpSocket::Read(std::string *msg) {
    int len = 0;
    read_((char*)&len, 4); // 先读取消息长度
    len = ntohl(len); // 字节序转换
    char *buf = new char[len+1];
    int n_read = read_(buf, len);
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

}