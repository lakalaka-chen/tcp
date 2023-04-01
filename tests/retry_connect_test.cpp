
#include "gtest/gtest.h"
#include "tcp_client.h"
#include "tcp_server.h"

#include <thread>
#include <future>
#include <memory>

using namespace tcp;

using TcpServerPtr = std::shared_ptr<TcpServer>;

/**
 * 第一次连接失败
 * 后续有需要再重新尝试连接
 * **/
TEST(RetryConnect, TEST1) {

    uint16_t port = 43251;
    std::atomic<bool> to_start{false};
    std::thread trigger([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        spdlog::info("Trigger, will start soon ... ");
        to_start.store(true);
    });

    auto create_server = [&]() -> TcpServerPtr {
        TcpServerPtr server_ptr = std::make_shared<TcpServer>("SERVER", port);
        while (!to_start.load()) { // 等待to_start变成true才开启服务器
            spdlog::info("it is not time to start, wait ... ");
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        server_ptr->Start();
        return server_ptr;
    };
    std::future<TcpServerPtr> server_ptr_future = std::async(create_server);

    TcpClient client;
    while (!client.IsConnected()) {
        client.ConnectTo("127.0.0.1", port, 10);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (trigger.joinable()) {
        trigger.join();
    }

//    TcpServerPtr server_ptr = server_ptr_future.get();
//    server_ptr->Close();

}