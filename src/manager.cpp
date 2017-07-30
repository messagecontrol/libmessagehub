#include "messagehub/manager.h"

Manager::Manager(MessageControl& mc) : msgctl(mc) {
    try {
        log = spdlog::stdout_color_mt("console");
        // This is where the log level is set
        log->set_level(spdlog::level::debug);
        spdlog::register_logger(log);
    } catch (const spdlog::spdlog_ex& e) {
        std::cerr << "[ERROR] Log was not initialized\n";
        std::cerr << "[ERROR] " << e.what() << "\n";
    }

    status = IDLE;
    activeConns = 0;
    inactiveConns = 0;
    stillCheck = true;
    stillNotify = true;
    stillMonitor = true;
    avg_reply_send_receive = 0l;
}

Manager::~Manager() {
    if (checkAllIfUp != nullptr && checkAllIfUp->joinable())
        checkAllIfUp->join();
    if (notifyCentral != nullptr && notifyCentral->joinable())
        notifyCentral->join();
    if (monitor != nullptr && monitor->joinable())
        monitor->join();
}

void Manager::run() {
    checkAllIfUp = std::make_unique<std::thread>(&Manager::_checkAllIfUp, this);
    notifyCentral = std::make_unique<std::thread>(&Manager::_notifyCentral, this);
    monitor = std::make_unique<std::thread>(&Manager::_monitor, this);
}

void Manager::_checkAllIfUp() {
    using namespace std::literals::chrono_literals;
    std::chrono::duration<double, std::milli> total_diffs{0};
    int count;
    auto start = std::chrono::high_resolution_clock::now();
    while(stillCheck) {
        count = 0;
        total_diffs = std::chrono::nanoseconds(0);
        for (std::pair<std::string, std::pair<std::string, bool> > connection : msgctl.connections) {
            if (connection.second.second)
                start = std::chrono::high_resolution_clock::now();
            msgctl.connect(connection.first, connection.second.first);
            if (connection.second.second)
                total_diffs = std::chrono::high_resolution_clock::now() - start;
        }
        avg_reply_send_receive = total_diffs.count() / count; 
        std::this_thread::sleep_for(2s);
    }
     
}


void Manager::_monitor() {
    int a_count, i_count;
    std::chrono::nanoseconds start, end, diff;
    using namespace std::literals::chrono_literals;
    while(stillMonitor) {
        a_count = 0;
        i_count = 0;
        for (std::pair<std::string, std::pair<std::string, bool> > connection : msgctl.connections) {
            if (connection.second.second)
                a_count++;
            else
                i_count++;
        }
        activeConns = a_count;
        inactiveConns = i_count;
        inQueueMsgs = msgctl.inQueue.size();
        outQueueMsgs = msgctl.outQueue.size();
        std::this_thread::sleep_for(2s); 
    }
}


void Manager::_notifyCentral() {
    while (stillNotify) {
    }
}
