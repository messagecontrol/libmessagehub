#include "messagehub/messagecontrol.h"


MessageControl::~MessageControl() {
    spdlog::drop_all();
    still_send = false;
    still_receive = false;
    if(sender->joinable())
        sender->join();
    if(receiver->joinable())
        receiver->join();
}

void MessageControl::run() {
    log->info("Initializing Threads");
    receiver = std::make_unique<std::thread>(std::thread(&MessageControl::_run_receiver, this));
    sender = std::make_unique<std::thread>(std::thread(&MessageControl::_run_sender, this));
}


void MessageControl::_run_sender() {
    log->debug("Started sender");
    zmq::context_t ctx(1);
    zmq::socket_t outSock(ctx, ZMQ_PUSH);
    while (still_send) {
        if (!outQueue.empty()) {
            log->trace("Connecting to {}", outQueue.front().first);
            if (outQueue.front().second->getFromHeader("type") != std::string("HANDSHAKE") &&
                outQueue.front().second->getFromHeader("type") != std::string("SHAKEHAND") &&
                (connections.find(outQueue.front().first) == connections.end() ||
                 !connections.at(outQueue.front().first).second)) {
                log->debug("{} is not registered as a connected endpoint, preforming handshake",
                           outQueue.front().first);
                connect(outQueue.front().first, outQueue.front().first, outQueue.front().second);
                outQueue.pop();
                continue;
            }
            outSock.connect("tcp://" + outQueue.front().first);
            log->trace("About to send: {}", outQueue.front().second->toString());
            std::string s = outQueue.front().second->toString();
            zmq::message_t zmsg(s.size());
            std::copy(s.begin(), s.end(), static_cast<char *>(zmsg.data()));
            try {
                outSock.send(zmsg);
            } catch (const zmq::error_t e) {
                log->error("Send Failed");
                log->error(e.what());
            }
            log->trace("Sent: {}", outQueue.front().first);
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
            log->trace("Receiving message");
            zmq::message_t zmsg;
            inSock.recv(&zmsg);
            Message_ptr m = std::make_shared<Message>(std::string(static_cast<char*>(zmsg.data()), zmsg.size()));
            log->trace(std::string(static_cast<char*>(zmsg.data()), zmsg.size()));
            log->trace("Received message with content: {}", m->toString());
            if (m->getFromHeader("type") == std::string("SHAKEHAND"))
                waitingOnShake = false;
            else if (m->getFromHeader("type") == std::string("HANDSHAKE")) {
                log->debug("Shaking {}'s hand", identity);
                Message_ptr msg = Message::empty();
                msg->setHeader("type", "SHAKEHAND");
                log->trace("Constructed message: {}", msg->toString());
                send(msg, m->getFromHeader("returnAddr"));
            } else
                inQueue.push(std::make_shared<Message>(zmsg));
        }
    }
    inSock.close();
    log->debug("Receiver stopped");
}

void MessageControl::send(Message_ptr m, const std::string &dst) {
    m->setHeader("returnAddr", returnAddr);
    m->setHeader("identity", identity);
    outQueue.push(std::make_pair(dst, m));
}

Message_ptr MessageControl::recv() {
    if (!inQueue.empty()) {
        Message_ptr msg = inQueue.front();
        inQueue.pop();
        return msg;
    }
    return nullptr;
}

bool MessageControl::handshake(const std::string &ip) {
    bool connected = false;
    log->trace("Attempting to initialize empty message");
    Message_ptr msg = Message::empty();
    log->trace("Initialized empty message");
    msg->setHeader("type", "HANDSHAKE");
    send(Message_ptr(msg), ip);
    bool timeout = false;
    std::thread timer(&MessageControl::_timer, this, 30, &timeout);
    timer.detach();
    while (waitingOnShake && !timeout) {}
    connected = !waitingOnShake;
    waitingOnShake = true;
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


void MessageControl::connect(const std::string &ipaddr, const int &port, const std::string &name, const Message_ptr msg) {
    std::string s = ipaddr + ":" + std::to_string(port);
    connect(s, name, msg);
}


void MessageControl::connect(const std::string &ipaddr, const std::string &name, const Message_ptr msg) {
    std::thread t(&MessageControl::_connect,this, ipaddr, name, msg);
    t.detach();
}

void MessageControl::_connect(const std::string &ipaddr, const std::string &name, const Message_ptr msg) {
    log->debug("Initiating handshake with {}", ipaddr);
    if (handshake(ipaddr)) {
        log->info("Successfully connected to {}", name);
        if (connections.find(ipaddr) == connections.end())
            addConnection(ipaddr, name);
        else
            connections[ipaddr].second = true;
        if (msg != nullptr)
            send(msg, ipaddr);
    } else {
        log->error("Connection timeout for {}", name);
    }
}


MessageControl::MessageControl(const std::string &name, const std::string &ipaddr, const int &bindingPort) {
    identity = name;
    port = bindingPort;
    returnAddr = ipaddr + ":" + std::to_string(port);
    initializeLog();
    still_send = true;
    still_receive = true;
    waitingOnShake = true;
    inQueue = std::queue<Message_ptr >();
    outQueue = std::queue<std::pair<std::string, Message_ptr > >();
    // FIXME: sleep is required to let the threads to start
    // If no fix is found then this can be left as is because in theory its initialized once
    // per program
    //std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
        log = std::make_shared<spdlog::logger>(identity, std::begin(log_sinks), std::end(log_sinks));//, 4096);
        // This is where the log level is set
        log->set_level(spdlog::level::trace);
        spdlog::register_logger(log);
    } catch (const spdlog::spdlog_ex& e) {
        std::cerr << "[ERROR] Log was not initialized\n";
        std::cerr << "[ERROR] " << e.what() << "\n";
    }
}

std::string MessageControl::getAddr() {
    return returnAddr;
}

std::string MessageControl::getIdentity() {
    return identity;
}

int MessageControl::getInQueueSize() {
    return inQueue.size();
}

int MessageControl::getOutQueueSize() {
    return outQueue.size();
}