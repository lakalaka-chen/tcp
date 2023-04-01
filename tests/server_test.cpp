
#include "gtest/gtest.h"
#include <thread>
#include <mutex>

#include "tcp_server.h"
#include "tcp_client.h"

namespace tcp {


using TcpServerPtr = std::shared_ptr<TcpServer>;

class EchoTwiceHandlerTest: public testing::Test {
public:
    static const uint16_t port = 1234;
    EchoTwiceHandlerTest(): server_(new TcpServer("Echo Twice Server", EchoTwiceHandlerTest::port)) {
        spdlog::set_level(spdlog::level::debug);
        server_->HandleReceiveData([](const std::string &recv_msg, std::string &reply_msg){
            reply_msg = recv_msg + " " + recv_msg;
        });
        server_->Start();
    }

    void Close() {
        if (server_) {
            server_->Close();
        }
    }

    bool IsRunning() {
        assert(server_ != nullptr);
        return server_->IsRunning();
    }

private:
    TcpServerPtr server_;
};


class Double2ServerTest: public testing::Test {
public:
    static const uint16_t port = 8888;
    Double2ServerTest(): server_(new TcpServer("Echo Twice Server", Double2ServerTest::port)) {
        spdlog::set_level(spdlog::level::debug);
        server_->HandleReceiveData([](const std::string &recv_msg, std::string &reply_msg){
            int num = std::stoi(recv_msg);
            reply_msg = std::string(std::to_string(2*num));
        });
        server_->Start();
    }

    void Close() {
        if (server_) {
            server_->Close();
        }
    }

    bool IsRunning() {
        assert(server_ != nullptr);
        return server_->IsRunning();
    }

private:
    TcpServerPtr server_;
};


TEST_F(Double2ServerTest, SimpleTest) {

    if (!IsRunning()) {
        spdlog::info("Try after servel seconds. ");
        return;
    }

    std::mutex mu;
    std::condition_variable cv;
    bool client_closed = false;

    std::thread client_thread([&](){
        std::unique_lock<std::mutex> lock(mu);
        TcpClient client;
        client.ConnectTo("127.0.0.1", port);
        std::string recv_msg;

        for (int i = 2; i < 1000; i ++) {
            std::string send_msg = std::to_string(i);
            client.SendMsg(send_msg);
            client.RecvMsg(&recv_msg);
            ASSERT_EQ(recv_msg, std::to_string(2*i));
        }


        client_closed = true;
        cv.notify_one();
    });
    client_thread.join();

    std::unique_lock<std::mutex> lock(mu);
    while (!client_closed) {
        cv.wait(lock);
    }
    Close();
}



TEST_F(EchoTwiceHandlerTest, SimpleEcho) {

    if (!IsRunning()) {
        spdlog::info("Try after servel seconds. ");
        return;
    }

    std::mutex mu;
    std::condition_variable cv;
    bool client_closed = false;

    std::thread client_thread([&](){
        std::unique_lock<std::mutex> lock(mu);
        TcpClient client;
        client.ConnectTo("127.0.0.1", EchoTwiceHandlerTest::port);
        std::string send_msg = "nihao";
        std::string recv_msg;
        client.SendMsg(send_msg);
        client.RecvMsg(&recv_msg);
        ASSERT_EQ(recv_msg, send_msg+" "+send_msg);
        client_closed = true;
        cv.notify_one();
    });
    client_thread.join();

    std::unique_lock<std::mutex> lock(mu);
    while (!client_closed) {
        cv.wait(lock);
    }
    Close();
}


TEST(RawServerTest, SimpleTest) {
    spdlog::set_level(spdlog::level::debug);
    TcpServer server("Simple", 9999);
    server.Start();

    if (!server.IsRunning()) {
        spdlog::info("Try after servel seconds. ");
        return;
    }

    std::mutex mu;
    std::condition_variable cv;
    bool client_closed = false;

    std::thread client_thread([&](){
        std::unique_lock<std::mutex> lock(mu);
        TcpClient client;
        client.ConnectTo("127.0.0.1", 9999);
        std::string send_msg = "nihao";
        std::string recv_msg;
        client.SendMsg(send_msg);
        client.RecvMsg(&recv_msg);
        ASSERT_EQ(recv_msg, send_msg);
        client_closed = true;
        cv.notify_one();
    });
    client_thread.join();

    std::unique_lock<std::mutex> lock(mu);
    while (!client_closed) {
        cv.wait(lock);
    }
    server.Close();

}


TEST(MultiClientTest, Test1) {
    spdlog::set_level(spdlog::level::debug);
    TcpServer server("Simple", 6666);
    server.Start();

    if (!server.IsRunning()) {
        spdlog::info("Try after servel seconds. ");
        return;
    }

    std::atomic<int> finish;
    finish.store(100);

    for (int i = 0; i < 100; i ++) {
        std::thread client_thread([&](){
            TcpClient client;
            client.ConnectTo("127.0.0.1", 6666);
            std::string send_msg = std::to_string(i);
            std::string recv_msg;
            client.SendMsg(send_msg);
            client.RecvMsg(&recv_msg);
            ASSERT_EQ(recv_msg, send_msg);
            finish.fetch_sub(1);
        });
        client_thread.join();
    }

    while (finish.load());

    server.Close();
}


/// 在服务器和客户端通信时, 关闭服务器, 查看是否会出问题
TEST(CloseTest, TEST1) {
    spdlog::set_level(spdlog::level::debug);
    TcpServer server("SERVER", 8888);
    server.Start();
}


}