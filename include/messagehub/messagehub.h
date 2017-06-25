#ifndef MESSAGEHUB_H
#define MESSAGEHUB_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <queue>
#include <string>
#include <thread>
#include <memory>
#include <zmq.hpp>

#include "messagehub/message.h"

#define DEFAULT_PORT 5555

class MessageHub {

protected:
    
    std::string identity;
    std::unique_ptr<std::thread> manager, msgProcessor, receiver, sender;
    std::queue<Message> inQueue;
    std::queue<std::pair<std::string, zmq::message_t> > outQueue;
    zmq::context_t context;
    zmq::socket_t inSock, outSock;
    int port;
    std::string hostAddr;
    bool still_process, still_receive, still_send;
    void _run();    
    void _run_sender();
    void _run_recevier();
    void process(std::string s);

public:
    
    MessageHub(std::string id, std::string hostip, int listenPort = DEFAULT_PORT);
    ~MessageHub();
    void run();
    std::string fullAddr();
    void send(std::string msg, std::string dst);
};

#endif
