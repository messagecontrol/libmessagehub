#include "messagehub/messagehub.h"

MessageHub::MessageHub(std::string id, std::string hostip, int listenPort) : context(1) , inSock(context, ZMQ_PULL), outSock(context, ZMQ_PUSH) {
    identity = id;
    inQueue = std::queue<Message>();
    outQueue = std::queue<std::pair<std::string, zmq::message_t> >();
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
    std::cout << "MSGPROCESSOR INITIALIZED\n";
    receiver = std::make_unique<std::thread>(std::thread(&MessageHub::_run_recevier, this));
    std::cout << "RECEIVER INITIALIZED\n";
    sender = std::make_unique<std::thread>(std::thread(&MessageHub::_run_sender, this));
    std::cout << "SENDER INITIALIZED\n";
}

void MessageHub::_run() {

    while (still_process) {
        if(!inQueue.empty()) {
            std::cout << "READING INBOX\n";
            //zmq::message_t zmsg;
            //zmsg.copy(&inQueue.front()); 
            
            //std::string s = std::string(static_cast<char*>(inQueue.front().data()), inQueue.front().size());
            Message msg = inQueue.front();
            std::cout << "PROCESSING MESSAGE\n";
            process(msg.toString());
            if (msg.needResponse()) {
                std::cout << "SENDING RESPONSE\n";
                Message m("YOUR MESSAGE WAS ACKNOWLEDGED");
                m.writeHeader(DELIMITERS_V1, "TEST", fullAddr()); 
                outQueue.push(std::make_pair(msg.returnAddr(), m.toZmqMsg())); 
                
                std::cout << "Message put in out queue\n";
            } 
            inQueue.pop();
            std::cout << "Popped inQueue\n";
        }
    }
}

void MessageHub::_run_sender() {
    zmq::context_t ctx(1);
    zmq::socket_t outSock(ctx, ZMQ_PUSH);
    while (still_send) {
        //std::cout << ":\n";
        //sleep(1);
        if (!outQueue.empty()) {
            std::cout << "Connecting to " << outQueue.front().first << " ... \n";
            outSock.connect("tcp://" + outQueue.front().first);
            std::cout << "OK\n";
            outSock.send(outQueue.front().second);
            outQueue.pop();
            //outSock.close();
        }
    }
}

void MessageHub::_run_recevier() {
    zmq::context_t ctx(1);
    zmq::socket_t inSock(ctx, ZMQ_PULL);
    std::cout << "ATTEMPTING TO BIND SOCKET\n";
    inSock.bind("tcp://*:" + std::to_string(port));
    std::cout << "BINDED INSOCK\n";
    while (still_receive) {
        std::cout << "0";
        sleep(1);
        zmq::message_t msg;
        inSock.recv(&msg);
        Message m(msg);
        inQueue.push(m);
    }
    inSock.close();
}


std::string MessageHub::fullAddr() {
    return identity + "::" + hostAddr + ":" + std::to_string(port);
}

void MessageHub::send(std::string m, std::string dst) {
    Message msg(m);
    msg.writeHeader(DELIMITERS_V1, "TEST2", fullAddr(), true);
    outQueue.push(std::make_pair(dst, msg.toZmqMsg()));
}
