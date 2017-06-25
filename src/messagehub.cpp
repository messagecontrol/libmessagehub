#include "messagehub/messagehub.h"

MessageHub::MessageHub(std::string id, std::string hostip, int listenPort) : context(1) , inSock(context, ZMQ_PULL), outSock(context, ZMQ_PUSH) {
    identity = id;
    inQueue = std::queue<std::unique_ptr<zmq::message_t> >();
    outQueue = std::queue<std::unique_ptr<std::pair<std::string, zmq::message_t> > >();
    still_process = true;
    still_send = true;
    still_receive = true;
    port = listenPort;
    hostAddr = hostip;
}

MessageHub::~MessageHub() {
    still_send = false;
    still_process = false;
    still_receive = false;
    if(msgProcessor->joinable())
        msgProcessor->join();
    if(sender->joinable())
        sender->join();
    if(receiver->joinable())
        receiver->join();

}

void MessageHub::process(std::string s) {
    std::cout << s << "\n";
}

void MessageHub::run() {
    msgProcessor = std::make_unique<std::thread>(std::thread(&MessageHub::_run, this));
    receiver = std::make_unique<std::thread>(std::thread(&MessageHub::_run_recevier, this));
    sender = std::make_unique<std::thread>(std::thread(&MessageHub::_run_sender, this));
}

void MessageHub::_run() {
    while (still_process) {
        if(!inQueue.empty()) {;
            Message msg(inQueue.front().get());
            process(msg.toString());
            if (msg.needResponse()) {
                Message m("YOUR MESSAGE WAS ACKNOWLEDGED");
                m.writeHeader(DELIMITERS_V1, "TEST", fullAddr()); 
                outQueue.push(std::make_unique<std::pair<std::string, zmq::message_t> >(std::make_pair(msg.returnAddr(), m.toZmqMsg()))); 
            } 
        }
    }
}

void MessageHub::_run_sender() {
    while (still_send) {
        if (!outQueue.empty()) {
            outSock.connect("tcp://" + outQueue.front()->first);
            outSock.send(outQueue.front()->second);
            outQueue.pop();
            outSock.close();
        }
    }
}

void MessageHub::_run_recevier() {
    inSock.bind("tcp//*:" + std::to_string(port));
    while (still_receive) {
        zmq::message_t msg;
        inSock.recv(&msg);
        inQueue.push(std::unique_ptr<zmq::message_t>(&msg));
    }
    inSock.close();
}


std::string MessageHub::fullAddr() {
    return identity + "::" + hostAddr + ":" + std::to_string(port);
}

void MessageHub::send(std::string m, std::string dst) {
    Message msg(m);
    msg.writeHeader(DELIMITERS_V1, "TEST2", fullAddr());
    outQueue.push(std::make_unique<std::pair<std::string, zmq::message_t> >(std::make_pair(dst, msg.toZmqMsg())));
}
