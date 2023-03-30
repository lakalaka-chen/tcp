#include "tcp_server.h"

namespace tcp {

/** TcpHandler **/

void TcpHandler::StartWorking() {
    std::string recv_msg;
    std::string reply_msg;
    int reply_size = 0;
    while (true) {
        int n_read = socket_->Read(&recv_msg);
        if (n_read == 0) {
            spdlog::debug("Client closed connection. ");
            break;
        } else if (n_read < 0) {
            spdlog::warn("Failed to receive data. ");
            break;
        }
        spdlog::debug("[{}:{}:{} Bytes]: {}", client_ip_, client_port_, n_read, recv_msg);

        func_(recv_msg, reply_msg);
        int n_reply = socket_->Write(reply_msg);
        if (n_reply < 0) {
            spdlog::warn("Failed to reply. ");
            break;
        }
    }
    socket_->Close();
}


/** TcpServer **/

bool TcpServer::Start() {
    bool success = init();
    if (!success) {
        spdlog::error("Server init failed. ");
        return false;
    }
    is_running_.store(true, std::memory_order_release);

    main_thread_ = std::thread([&](){
        while(is_running_.load(std::memory_order_acquire)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_fd = accept(server_fd_, (struct sockaddr*)(&client_addr), &addr_len);
            if (client_fd < 0) {
                spdlog::warn("Failed to accept connection");
                break;
            }

            char ip[32];
            const char *client_ip = inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), ip, sizeof(ip));
            uint16_t port = ntohs(client_addr.sin_port);
            spdlog::debug("Incoming: [{}:{}]", client_ip, port);

            std::shared_ptr<TcpSocket> socket_ptr = std::make_shared<TcpSocket>(client_fd);
            std::shared_ptr<TcpHandler> handler_ptr(new TcpHandler(socket_ptr, ip, port, func_));

            std::thread handler_thread([](std::shared_ptr<TcpHandler> handler){
                handler->StartWorking();
            }, std::move(handler_ptr));
            handler_thread.detach();
        }
        spdlog::debug("Server main thread exited");
    });


    return true;
}

bool TcpServer::init() {
    server_fd_ = TcpSocket::CreateSocket();
    if (server_fd_ < 0) {
        spdlog::error("Failed to create socket. ");
        return false;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        spdlog::error("Failed to bind socket. ");
        return false;
    }

    if (listen(server_fd_, capacity_) < 0) {
        spdlog::error("Failed to listen socket. ");
        return false;
    }

    spdlog::info("Server start listening [{}]. ", port_);
    return true;
}



TcpServer::TcpServer(std::string name, uint16_t port, size_t capacity)
    : name_(std::move(name)), port_(port), is_running_(false), capacity_(capacity), func_(default_func) {

}


TcpServer::~TcpServer() {
    if (main_thread_.joinable()) {
        main_thread_.join();
    }
    spdlog::info("{} closed", name_);
    Close();
}

bool TcpServer::IsRunning() {
    return is_running_.load(std::memory_order_relaxed);
}

void TcpServer::RegisterAction(std::function<void(const std::string &, std::string &)> func) {
    func_ = func;
}


void TcpServer::stop() {
    is_running_.store(false, std::memory_order_release);
    if (server_fd_ > 0){
        close(server_fd_);
    }
}


bool TcpServer::Close() {
    stop();
    return true;
}


}