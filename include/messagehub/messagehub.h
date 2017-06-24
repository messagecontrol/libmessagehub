#ifndef MESSAGEHUB_H
#define MESSAGEHUB_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <thread>
#include <memory>
#include <zmq.hpp>

#include "messagehub/message.h"

class MessageHub {

protected:
    
    std::vector<zmq::socket_t> sockets;
    std::string identity;
    std::thread sender, manager;
    std::vector<std::unique_ptr<std::thread> > receviers;
    std::priority_queue<std::unique_ptr<Message> > msgQueue;
    
    void _run();    

public:
    
    MessageHub(std::string id = "anon");
    ~MessageHub();
    void run();
};

#endif
