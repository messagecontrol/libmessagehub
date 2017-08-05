#ifndef MANAGER_H
#define MANAGER_H

#include <thread>
#include <memory>
#include <chrono>

#include <spdlog/logger.h>

#include "messagehub/message.h"
#include "messagehub/messagecontrol.h"


typedef std::chrono::time_point<std::chrono::high_resolution_clock,
                                std::chrono::milliseconds> Millisecond;

enum Status {
    IDLE, // Nothing is happening
    GOOD, // Everything works well but can be better
    OK, // Everything works at minimal
    BAD, // Lots of holds and warnings
    ERROR // An error causing an unusable state
};

class MessageControl;

class Manager {
    protected:
        Status status;
        std::shared_ptr<MessageControl> msgctl;
        std::unique_ptr<std::thread> checkAllIfUp, notifyCentral, monitor;
        bool stillCheck, stillNotify, stillMonitor;
        std::shared_ptr<spdlog::logger> log;
        int activeConns, inactiveConns, inQueueMsgs, outQueueMsgs;
        long avg_reply_send_receive;

        void _checkAllIfUp(); // Run in thread 
        void _notifyCentral(); // Run in thread
        void _monitor(); // Run in thread

    public:
        Manager(std::shared_ptr<MessageControl> mc);
        ~Manager();
        void run();
};

#endif
