#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <unistd.h>


namespace tcp {

class TcpSocket {
private:
    int socket_fd_;
public:
    static int CreateSocket() {
        return socket(AF_INET, SOCK_STREAM, 0);
    }
    explicit TcpSocket(int fd): socket_fd_(fd) {}
    ~TcpSocket() {}

    // 收数据, 存入*msg, 返回读取的数据量
    int Read(std::string *msg);
    // 发数据, 把msg里面的内容发过去, 返回发送数据量
    int Write(const std::string &msg);
    // 关闭连接
    void Close() { close(socket_fd_); }
    int GetSocketFd() const { return socket_fd_; }

private:
    int write_(const char* data, int data_size) {
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
    int read_(char *buf, int to_read) {
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
};

}