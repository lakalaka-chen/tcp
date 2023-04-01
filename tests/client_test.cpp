
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "tcp_client.h"
#include "tcp_server.h"


#include <thread>
#include <mutex>
#include <atomic>

namespace tcp {


TEST(CLientTest, Empty) {

    spdlog::set_level(spdlog::level::debug);


    std::string ip = "127.0.0.0.1";
    uint16_t port = 8888;

    TcpClient client;
    bool status;

    status = client.ConnectTo(ip, port);
    ASSERT_EQ(true, status);

    std::string send_msg;
    std::string recv_msg;
    std::cout << "Input you message: ";
    while (std::cin >> send_msg) {
        status = client.SendMsg(send_msg);
        ASSERT_EQ(true, status);
        status = client.RecvMsg(&recv_msg);
        ASSERT_EQ(true, status);
        spdlog::debug("Receive: {}", recv_msg);
    }

    spdlog::info("Finish sending");
    client.Close();

}


}



