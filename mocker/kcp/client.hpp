#pragma once
#include <fmt/core.h>
#include <hv/UdpClient.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <hv/json.hpp>
#include <vector>

struct KcpClient {
    KcpClient(const char* jpath) {
        std::ifstream file{jpath};
        auto j = nlohmann::json::parse(file);
        // connection settings
        remote_host = j["host"];
        remote_port = j["port"];
        // kcp settings
        kcp_setting_.conv = j["conv"];
        kcp_setting_.nodelay = j["nodelay"];
        kcp_setting_.interval = j["interval"];
        kcp_setting_.fastresend = j["resend"];
        kcp_setting_.nocwnd = j["nc"];
        kcp_setting_.sndwnd = j["sndwnd"];
        kcp_setting_.rcvwnd = j["rcvwnd"];
        // local host for UDS
        local_host = std::tmpnam(nullptr);
    }
    ~KcpClient() {
        std::filesystem::remove(local_host);
    }

    void start() {
        if (auto sockfd = client_.createsocket(remote_port, remote_host.c_str()); sockfd < 0) {
            fmt::println("Failed to create socket");
            return;
        }

        // deal wit UDS path
        if (remote_port < 0) {
            client_.bind(remote_port, local_host);
        }

        client_.onMessage = [](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
        };
        client_.onWriteComplete = [](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {

        };

        // set kcp
        client_.setKcp(&kcp_setting_);
        client_.start();
    }

    void subscribe(std::vector<std::string> const& symbols) {
    }

    void wait() {
        while (getchar() != '\n');
    }

   public:
    int remote_port;
    std::string remote_host;
    const char* local_host;

   private:
    kcp_setting_t kcp_setting_;
    hv::UdpClient client_;
};