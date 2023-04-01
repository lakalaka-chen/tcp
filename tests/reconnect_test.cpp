
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "tcp_server.h"
#include "tcp_client.h"

#include <atomic>
#include <thread>
#include <mutex>

using namespace tcp;

TEST(ReConnectTest, ClientEnd) {

    spdlog::set_level(spdlog::level::debug);
    uint16_t port = 12345;

    std::atomic<bool> to_start{false};

    std::thread server_thread([&]() {
        TcpServer server("SERVER", port);
        while (!to_start.load()) { // 等待to_start变成true才开启服务器
            spdlog::debug("it is not time to start, wait ... ");
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        server.Start();
    });


    std::thread trigger([&]() { // 这个线程过10s开启to_start
        std::this_thread::sleep_for(std::chrono::seconds(2));
        spdlog::debug("Trigger, will start soon ... ");
        to_start.store(true);
    });

    TcpClient client;
    client.ConnectTo("127.0.0.1", port, 0);

    std::string send_msg, recv_msg;
    std::cout << "Input: ";
    while (std::cin >> send_msg) {
        if (!client.SendMsg(send_msg)) break;
        if (!client.RecvMsg(&recv_msg)) break;
        spdlog::info("Receive: {}", recv_msg);
        std::cout << "Input: ";
    }


    if (trigger.joinable()) {
        trigger.join();
    }
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

