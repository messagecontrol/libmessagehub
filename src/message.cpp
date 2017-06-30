#include "messagehub/message.h"

Message::Message(std::string s) {
    _string = s;
}

Message::Message(zmq::message_t & zmsg) {
    std::string s = std::string(static_cast<char*>(zmsg.data()), zmsg.size());
    parseString(s);
}

std::string Message::toString() {
    return std::to_string(_header.delimiters) + ";"
           + _header.msgType + ";"
           + _header.originAddr + ";"
           + std::to_string(_header.needResponse) + ";"
           + "END_HEAD;" 
           + _string
           + ";END_MSG;";
}

zmq::message_t Message::toZmqMsg() {
    const std::string s = toString();
    zmq::message_t msg(s.size());
    std::copy(s.begin(), s.end(), static_cast<char *>(msg.data()));
    return msg;
}

void Message::parseString(std::string s) {
    size_t pos = 0;
    std::string token;
    bool end_header = false;
    bool end_msg = true;
    int iteration = 0;
    while ((pos = s.find(DELIMITER)) != std::string::npos) {
        token = s.substr(0, pos);
        if (end_header && end_msg) {
            _string = token;
            end_msg = false;
        } else if (!end_header) {
            switch (iteration) {
                case 0:
                   _header.delimiters = std::stoi(token);
                   break;
                case 1:
                   _header.msgType = token;
                   break;
                case 2:
                   _header.originAddr = token;
                   break;
                case 3:
                   _header.needResponse = std::stoi(token);
                   break;
                case 4:
                   if (std::strcmp(token.c_str(),"END_HEAD") == 0)
                       throw MessageFormatException("Expected \"END_HEAD\"", s);
                   end_header = true;
                   break;
            }
        } else {
            if (std::strcmp(token.c_str(), "END_MSG") == 0) {
                throw MessageFormatException("Expected \"END_MSG\"", s);
            }
        }
        s.erase(0, pos + DELIMITER_SIZE);
        iteration++;
    }
}


bool Message::needResponse() {
    return _header.needResponse;
}

std::string Message::returnAddr() {
    std::string s = _header.originAddr;
    size_t pos =0;
    std::string token;
    while ((pos = s.find("::")) != std::string::npos) {
        token = s.substr(0,pos);
        s.erase(0, pos + 2);
    }
    return s;
}

void Message::writeHeader(int delimiters, std::string msg_type, std::string full_addr, bool need_response) {
    _header.delimiters = delimiters;
    _header.msgType = msg_type;
    _header.originAddr = full_addr;
    _header.needResponse = need_response;
}
