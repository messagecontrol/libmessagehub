#ifndef MESSAGEHUB_H
#define MESSAGEHUB_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <rapidjson/reader.h>
#include "messagehub/message.h"
#include "messagehub/manager.h"

class Manager;

class MessageControl {
    friend Manager;
    protected:
        std::string returnAddr, identity;
        int port;

        // Control Bools
        bool still_receive, still_send;

        // Status Bools
        bool waitingOnShake;

        std::unique_ptr<std::thread> receiver, sender;

        std::queue<Message_ptr > inQueue;

        std::queue<std::pair<std::string, Message_ptr > > outQueue;

        std::map<std::string, std::pair<std::string, bool> > connections;
        std::shared_ptr<spdlog::logger> log;
        std::vector<spdlog::sink_ptr> log_sinks;
        std::shared_ptr<spdlog::sinks::sink> file_error_sink;
        std::shared_ptr<spdlog::sinks::ansicolor_sink<std::mutex> > color_console_sink;

        void _run_sender();
        void _run_receiver();

        void addConnection(const std::string &ipaddr, const std::string &name);
        void addConnection(const std::string &ipaddr, const int &port, const std::string &name);

        void _timer(int time, bool *flag);
        bool handshake(const std::string &addr);

        void _connect(const std::string &ipaddrWithPort, const std::string &name, const Message_ptr = nullptr);

    public:

        MessageControl(const std::string &id, const std::string &hostAddr, const int &bindingPort);
        ~MessageControl();

        void run();
        void initializeLog();
        std::string getIdentity();

        std::string getAddr();

        int getInQueueSize();
        int getOutQueueSize();

        void send(Message_ptr msg, const std::string &dst);

        Message_ptr recv();

        void connect(const std::string &ipaddr, const int &port, const std::string &name, const Message_ptr msg = nullptr);
        void connect(const std::string &ipaddrWithPort, const std::string &name, const Message_ptr msg = nullptr);
};

#endif
