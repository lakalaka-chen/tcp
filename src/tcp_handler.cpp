#include "tcp_handler.h"
#include "spdlog/spdlog.h"

namespace tcp {

/** TcpHandler **/

void TcpHandler::StartWorking() {
    std::string recv_msg;
    std::string reply_msg;
    int reply_size = 0;
    while (true) {
        int n_read = socket_->Read(&recv_msg);
        if (n_read == 0) {
//            spdlog::debug("Client closed connection. ");
            break;
        } else if (n_read < 0) {
//            spdlog::warn("Failed to receive data. ");
//            break;
        }
//        spdlog::debug("[{}:{}:{} Bytes]: {}", client_ip_, client_port_, n_read, recv_msg);

        func_(recv_msg, reply_msg);
        if (reply_msg == "") {
            spdlog::debug("结点[{}]拒绝回复", server_name_);
            continue; // 构造空的reply message, 表示忽略这次收到的数据包
        }
        int n_reply = socket_->Write(reply_msg);
        if (n_reply < 0) {
//            spdlog::warn("Failed to reply. ");
            break;
        }
    }
    socket_->Close();
}

}