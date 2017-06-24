#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include <zmq.hpp>

class Message {
    
protected:
    std::string _string;
    
public:
    Message(std::string);
    Message(zmq::message_t zmsg);

    zmq::message_t toZmqMsg();

    std::string toString();
};


#endif
