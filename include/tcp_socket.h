#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <mutex>


namespace tcp {

class TcpSocket {
private:
    int socket_fd_;
public:
    static int CreateSocket();
    explicit TcpSocket(int fd);
    ~TcpSocket();

    int Read(std::string *msg);
    int Write(const std::string &msg);

    void Close();
    int GetSocketFd() const { return socket_fd_; }

private:

    int write_(const char* data, int data_size);
    int read_(char *buf, int to_read);
};

}