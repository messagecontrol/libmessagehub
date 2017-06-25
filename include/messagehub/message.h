#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <cassert>
#include <iostream>
#include <zmq.hpp>

#define DELIMITER ";"
#define DELIMITERS_V1 6

#define DELIMITER_SIZE 1
typedef struct _header_t {
    int delimiters;
    std::string msgType;
    std::string originAddr;
    bool needResponse;
} header_t;


class Message {
    
protected:
    header_t _header;
    std::string _string; 
    void parseString(std::string s);

public:
    Message(std::string);
    Message(zmq::message_t &zmsg);
    zmq::message_t toZmqMsg();
    std::string toString();
    bool needResponse();
    std::string returnAddr();
    void writeHeader(int delimiters, std::string msg_type, std::string fullAddr, bool nr=0);
};


#endif
