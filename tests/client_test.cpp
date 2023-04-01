
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


//TEST(ClientTest, AutoReConnectTest) {
//
//    spdlog::set_level(spdlog::level::debug);
//
//    uint16_t port = 12345;
//
//    std::atomic<bool> to_start{false};
//
//    std::thread server_thread([&](){
//        TcpServer server("SERVER", port);
//
//        while (!to_start.load()) { // 等待to_start变成true才开启服务器
//            std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        }
//
//        server.Start();
//    });
//
//
//    std::thread trigger([&](){ // 这个线程过10s开启to_start
//        std::this_thread::sleep_for(std::chrono::seconds(5));
//        to_start.store(true);
//    });
//
//    TcpClient client;
//    client.ConnectTo("127.0.0.1", port);
//
//
//    if (trigger.joinable()) {
//        trigger.join();
//    }
//}


}



