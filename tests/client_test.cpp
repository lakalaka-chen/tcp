
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "tcp_client.h"

namespace tcp {


TEST(CLientTest, Empty) {

    spdlog::set_level(spdlog::level::debug);



    std::string ip = "127.0.0.0.1";
    uint16_t port = 1236;

    TcpClient client;
    bool status;

    status = client.ConnectTo(ip, port);
    ASSERT_EQ(true, status);

    std::string send_msg = "Empty Test";
    std::string recv_msg;

    status = client.SendMsg(send_msg);
    ASSERT_EQ(true, status);
    status = client.RecvMsg(&recv_msg);
    ASSERT_EQ(true, status);
    spdlog::debug("Receive: {}", recv_msg);
    ASSERT_EQ(send_msg, recv_msg);

    spdlog::info("Finish sending");
    client.Close();

}

}



