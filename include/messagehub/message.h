#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
// TODO: Remove assertions and add exceptions
#include <cassert>
// TODO: Use a logging module to log messages
#include <zmq.hpp>

// This is the delimiter used in the message
// TODO: Import this from a config file
#define DELIMITER ";"

// The number of delimiters based on message version. The version of the 
// message can be determined by the delimiters value in the header.
// TODO: Import this from a config file
#define DELIMITERS_V1 6

// This is the number of characters used in the delimiter
#define DELIMITER_SIZE 1

// This structure will encapsulate the header
typedef struct _header_t {
    int delimiters;
    std::string msgType;
    std::string originAddr;
    bool needResponse;
} header_t;


// TODO: Create an interface / abstract class to allow for custom messages
class Message {
    
protected:
    header_t _header;
    std::string _string; 
    // This is protected because it will edit the header
    void parseString(std::string s);

public:
    Message(std::string);

    // Taking reference because the message content is copied
    Message(zmq::message_t &zmsg);
    
    zmq::message_t toZmqMsg();

    // This returns the entire message in the complete format with header
    std::string toString();
    
    // Gives the header value for needResponse
    bool needResponse();

    // This gives the return address in the format of 192.168.1.123:PORT
    std::string returnAddr();

    // Write the header with the given options
    void writeHeader(int delimiters, std::string msg_type, std::string fullAddr, bool nr=0);

    // TODO: Need a getter and setter for the underlying message
    //       It hasn't been implemented yet because there is a possibility of
    //       introducing size constraints in the message specification.
};


#endif
