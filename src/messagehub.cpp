#include "messagehub/messagehub.h"


MessageControl::~MessageControl() {
    spdlog::drop_all();
    still_send = false;
    still_manage = false;
    still_receive = false;
    if(manager->joinable())
        manager->join();
    if(sender->joinable())
        sender->join();
    if(receiver->joinable())
        receiver->join();
}
/*
void MessageHub::process(std::string s) {
    std::cout << s << "\n";
}

void MessageHub::process(Message &msg) {
    if (std::strcmp(msg.getMsg().c_str(), "HANDSHAKE") == 0) {
        std::cout << "[INFO] Recevied handshake request\n";
        send("SHAKEHAND", msg.returnAddr());
    } else if (std::strcmp(msg.getMsg().c_str(), "SHAKEHAND") == 0) {
        waitingOnShake = false;
    } else {
        process(msg.getMsg());
    }
}*/

void MessageControl::run() {
    log->info("Initializing Threads");
    manager = std::make_unique<std::thread>(std::thread(&MessageControl::_run_manager, this));
    receiver = std::make_unique<std::thread>(std::thread(&MessageControl::_run_receiver, this));
    sender = std::make_unique<std::thread>(std::thread(&MessageControl::_run_sender, this));
}
/*
void _run() {

    while (still_process) {
        if(!inQueue.empty()) {
            std::cout << "READING INBOX\n";
            //zmq::message_t zmsg;
            //zmsg.copy(&inQueue.front()); 
            
            //std::string s = std::string(static_cast<char*>(inQueue.front().data()), inQueue.front().size());
            Message msg = inQueue.front();
            std::cout << "PROCESSING MESSAGE\n";
            process(msg);
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
}*/

void MessageControl::_run_manager() {
    log->debug("Started manager");
    while (still_manage) {
    
    }
    log->debug("Manager ended");
}

void MessageControl::_run_sender() {
    log->debug("Started sender");
    zmq::context_t ctx(1);
    zmq::socket_t outSock(ctx, ZMQ_PUSH);
    while (still_send) {
        if (!outQueue.empty()) {
            outSock.connect("tcp://" + outQueue.front().first);
            outSock.send(outQueue.front().second.toZmqMsg());
            outQueue.pop();
        }
    }
    log->debug("Sender ended");
}

void MessageControl::_run_receiver() {
    log->debug("Started recevier");
    zmq::context_t ctx(1);
    zmq::socket_t inSock(ctx, ZMQ_PULL);
    zmq::pollitem_t items[] = {{inSock, 0, ZMQ_POLLIN, 0}};
    inSock.bind("tcp://*:" + std::to_string(port));
    while (still_receive) {
        zmq::poll(&items[0], 1, 10);
            if (items[0].revents & ZMQ_POLLIN) {    
            zmq::message_t msg;
            inSock.recv(&msg);
            JSONMessage m(msg);
            if (m.getFromHeader("type") == std::string("SHAKEHAND"))
                waitingOnShake = false;
            else if (m.getFromHeader("type") == std::string("HANDSHAKE")) {
                JSONMessage msg = JSONMessage::empty();
                msg.setHeader("type", "SHAKEHAND");
                send(msg, msg.getFromHeader("returnAddr"));  
            } else
                inQueue.push(m);
        }
    }
    inSock.close();
    log->debug("Receiver stopped");
}



void MessageControl::send(JSONMessage &m, const std::string &dst) {
    m.setHeader("returnAddr", returnAddr);
    outQueue.push(std::make_pair(dst, m));
}

bool MessageControl::handshake(const std::string &ip) {
    bool connected = false;
    JSONMessage msg = JSONMessage::empty();
    msg.setHeader("type", "HANDSHAKE");
    send(msg, ip);
    bool timeout = false;
    std::thread timer(&MessageControl::_timer, this, 15, &timeout);
    timer.detach();
    while (waitingOnShake && !timeout) {}
    connected = !waitingOnShake;
    waitingOnShake = true;
    if (timeout) std::cout << "[ERROR] Handshake timed out\n";
    return connected;
}


void MessageControl::_timer(int time, bool * flag) {
    sleep(time);
    *flag = true;
}

void MessageControl::addConnection(const std::string & ipaddr, const int &port, const std::string & name) {
    std::string s = ipaddr + ":" + std::to_string(port);
    addConnection(s, name);
}

void MessageControl::addConnection(const std::string & ipaddr, const std::string & name) {
    connections.insert(std::make_pair(ipaddr, std::make_pair(name, true)));
}


bool MessageControl::connect(const std::string &ipaddr, const int &port, const std::string &name) {
    std::string s = ipaddr + ":" + std::to_string(port);
    return connect(s, name);
}

bool MessageControl::connect(const std::string &ipaddr, const std::string &name) {
    if (handshake(ipaddr)) {
        log->info("Successfully connected to {}", name);
        addConnection(ipaddr, name);
        return true;
    } else {
        std::cout << "[ERROR] Connection timeout for " + name + "\n";
        return false;
    }
}

MessageControl::MessageControl(const std::string &name, const std::string &ipaddr, const int &bindingPort) {
    identity = name;
    port = bindingPort;
    returnAddr = ipaddr + std::to_string(port);
    initializeLog();
    still_send = true;
    still_manage = true;
    still_receive = true;
    waitingOnShake = false;
    inQueue = std::queue<JSONMessage>();
    outQueue = std::queue<std::pair<std::string, const JSONMessage> >();
    run();
    // FIXME: sleep is required to let the threads to start
    // If not fix is found then this can be left as is because in theory its initialized once
    // per program 
    //std::this_thread::sleep_for(std::chrono::milliseconds(0));
}


void MessageControl::initializeLog() {
    try {
        log_sinks = std::vector<spdlog::sink_ptr>();
        // The following requires the file to exist
        // file_error_sink = std::make_shared<spdlog::sinks::simple_file_sink_mt>("logs/messagecontrol.log");
        std::shared_ptr<spdlog::sinks::stdout_sink_mt> stdout_sink = spdlog::sinks::stdout_sink_mt::instance();
        color_console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
        //log_sinks.push_back(file_error_sink);
        log_sinks.push_back(color_console_sink);
        log = std::make_shared<spdlog::async_logger>("MessageControl", std::begin(log_sinks), std::end(log_sinks), 4096);
        // This is where the log level is set
        log->set_level(spdlog::level::trace);
        spdlog::register_logger(log);
    } catch (const spdlog::spdlog_ex& e) {
        std::cerr << "[ERROR] Log was not initialized\n";
        std::cerr << "[ERROR] " << e.what() << "\n";
    }
}

std::string MessageControl::getAddr() {
    return returnAddr + std::to_string(port);
}
