
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "tcp_client.h"
#include "tcp_server.h"


#include <thread>
#include <memory>
#include <mutex>
#include <atomic>

namespace tcp {

using TcpServerPtr = std::shared_ptr<TcpServer>;
using TcpClientPtr = std::shared_ptr<TcpClient>;

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


/*
 * 测试服务器关闭连接能不能正常断开客户端的socket
 * 测试服务器重启后, 客户端能不能正常建立新连接
 * */
TEST(ClientTest, ServerDisconnect) {
    spdlog::set_level(spdlog::level::debug);
    uint16_t port = RandomPort();
    auto create_server = [port]() {
        TcpServerPtr server = std::make_shared<TcpServer>("SERVER", port);
        if (!server->Start()) {
            spdlog::error("服务器启动失败, 可能是端口已经被占用[{}]", port);
            server.reset();
        } else {
            spdlog::debug("服务器已启动, 正在监听端口[{}]", port);
        }

        return server;
    };
    std::future<TcpServerPtr> fu = std::async(create_server);
    TcpServerPtr server = fu.get();
    if (server == nullptr) {
        return;
    }

    // 主线程启动客户端
    TcpClientPtr client = std::make_shared<TcpClient>();
    if (!client->ConnectTo("127.0.0.1", port, 10)) {
        spdlog::error("客户端连接失败");
        return;
    }
    spdlog::debug("客户端连接服务器成功");

    auto client_loop = [port, client]() {
        std::string message;
        std::string recv_msg;
        spdlog::debug("客户端LOOP启动");
        bool connected = true;
        int times = 0;
        while (true) {
            uint16_t send_int = RandomPort(); // 生成一个随机端口号当作发送内容
            message = std::to_string(send_int);
            if (!client->SendMsg(message)) {
                spdlog::debug("发送失败");
                connected = false;
            } else {
                times = 0;
                spdlog::debug("客户端发送[{}]", message);
            }
            if (!client->RecvMsg(&recv_msg)) {
                spdlog::debug("收取失败");
                connected = false;
            } else {
                times = 0;
                spdlog::debug("客户端收到[{}]", recv_msg);
                if (message != recv_msg) {
                    spdlog::error("收到消息出现错误, 希望收到[{}], 但是收到[{}]", message, recv_msg);
                    break;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            if (!connected) {
                times ++;
                if (client->ConnectTo("127.0.0.1", port, 10)) {
                    spdlog::debug("客户端重新连接成功");
                    connected = true;
                } else {
                    spdlog::debug("第[{}]次客户端重新连接失败, 稍后会重新尝试", times);
                    connected = false;
                    if (times == 3) {
                        spdlog::debug("重连次数达到极限, 服务器可能不会再重启");
                        break;
                    }
                }
            }
        }
        spdlog::debug("客户端LOOP即将退出");
    };

    std::thread client_mainloop(client_loop);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    server->Close();
    spdlog::debug("服务器关闭一会...");
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    if(server->Start()) {
        spdlog::debug("服务器重启成功");
    } else {
        spdlog::debug("服务器重启失败");
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    server->Close();
    spdlog::debug("服务器完全关闭...");

    if (client_mainloop.joinable()) {
        client_mainloop.join();
    }
    // 其实没必要手动Close()
    server->Close();
}



//TEST(ClientTest, ClientAutoReconnect) {
//    spdlog::set_level(spdlog::level::debug);
//    uint16_t port = RandomPort();
//    auto create_server = [port]() {
//        TcpServerPtr server = std::make_shared<TcpServer>("SERVER", port);
//        if (!server->Start()) {
//            spdlog::error("服务器启动失败, 可能是端口已经被占用[{}]", port);
//            server.reset();
//        } else {
//            spdlog::debug("服务器已启动, 正在监听端口[{}]", port);
//        }
//
//        return server;
//    };
//    std::future<TcpServerPtr> fu = std::async(create_server);
//    TcpServerPtr server = fu.get();
//    if (server == nullptr) {
//        return;
//    }
//
//    // 主线程启动客户端
//    TcpClientPtr client = std::make_shared<TcpClient>();
//    if (!client->ConnectTo("127.0.0.1", port, 10)) {
//        spdlog::error("客户端连接失败");
//        return;
//    }
//    spdlog::debug("客户端连接服务器成功");
//
//    auto client_loop = [port, client]() {
//        std::string message;
//        std::string recv_msg;
//        spdlog::debug("客户端LOOP启动");
//        int error_times = 0;
//        int error_limit = 10;
//        while (true) {
//            uint16_t send_int = RandomPort(); // 生成一个随机端口号当作发送内容
//            message = std::to_string(send_int);
//            if (!client->SendMsg(message)) {
//                spdlog::debug("发送失败");
//                error_times ++;
//            } else {
//                error_times = 0;
//                spdlog::debug("客户端发送[{}]", message);
//            }
//            if (!client->RecvMsg(&recv_msg)) {
//                spdlog::debug("收取失败");
//                error_times ++;
//            } else {
//                error_times = 0;
//                spdlog::debug("客户端收到[{}]", recv_msg);
//                if (message != recv_msg) {
//                    spdlog::error("收到消息出现错误, 希望收到[{}], 但是收到[{}]", message, recv_msg);
//                    break;
//                }
//            }
//            std::this_thread::sleep_for(std::chrono::milliseconds(800));
//            if (error_times == error_limit) {
//                break;
//            }
//        }
//        spdlog::debug("客户端LOOP即将退出");
//    };
//
//    std::thread client_mainloop(client_loop);
//
//    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    server->Close();
//    spdlog::debug("服务器关闭一会...");
//    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    if(server->Start()) {
//        spdlog::debug("服务器重启成功");
//    } else {
//        spdlog::debug("服务器重启失败");
//    }
//
//
//    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    server->Close();
//    spdlog::debug("服务器完全关闭...");
//
//    if (client_mainloop.joinable()) {
//        client_mainloop.join();
//    }
//    // 其实没必要手动Close()
//    server->Close();
//}


}



