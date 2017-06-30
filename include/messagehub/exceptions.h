#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

#include <zmq.hpp>

class MsgHubException: public std::exception {
    protected:
        std::string _msg;
    public:
        inline explicit MsgHubException(const char * m): _msg(m) {}
        inline explicit MsgHubException(const std::string & m): _msg(m) {}
        virtual ~MsgHubException() throw() {}
        virtual const char * what() const throw() {
            return _msg.c_str();
        }
};


class MessageFormatException: public MsgHubException {
    protected:
        std::string _incorrect_msg;
    public:
        
        // The incorrect message is set to "[UNSET]" so that if an incorrect message is empty it will still print showing that it is empty
        inline explicit MessageFormatException(const std::string & m, const std::string & inc = "[UNSET]"): MsgHubException(m), _incorrect_msg(inc) {}
        inline explicit MessageFormatException(const char * m, const char * inc = "[UNSET]"): MsgHubException::MsgHubException(m), _incorrect_msg(inc) {}

        virtual const char * what() const throw() {
            if (std::strcmp(_incorrect_msg.c_str(), "[UNSET]") == 0) {
                return _msg.c_str();
            } else {
                std::string msg = _msg + " Message:\n" + _incorrect_msg + "\n";
                return msg.c_str();
            }
        }
};


class ConfigurationException: public MsgHubException {
    // TODO: Need to implement this probably after the ability to configure settings has been implemented
};

#endif

