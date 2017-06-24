#include "messagehub/messagehub.h"

MessageHub::MessageHub(std::string id) {
    sockets = std::vector<zmq::socket_t>();
    msgQueue = std::vector<std::unique_ptr<std::thread> >();
    identity = id;
}

MessageHub::~MessageHub() {
    while(!msgQueue.empty()) {
        if (msgQueue.back()->joinable())
            msgQueue.back()->join();
        msgQueue.pop_back();
    }
}
