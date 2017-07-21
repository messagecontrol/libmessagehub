#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <zmq.hpp>
#include "spdlog/spdlog.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "messagehub/exceptions.h"


class Handler;
class Message;


typedef std::shared_ptr<Message> Message_ptr;

class BaseMessage {
    protected:
        virtual void parseString(const std::string &s) = 0;

    public:
        virtual ~BaseMessage() {};
        virtual zmq::message_t toZmqMsg() const = 0;
        virtual std::string toString() const = 0;
        virtual std::string returnAddr() const= 0;
        virtual int getPriority() const= 0;
};



class Message: public BaseMessage {

    friend class Handler;

    protected:
        void parseString(const std::string & s);
        std::map<std::string, std::string> header, body;
        std::string currentKey;
        bool editingHeader;
        void set(const std::string &val);

    public:
        Message(const std::string &s);
        Message(zmq::message_t &zmsg);
        Message(const Message &msg);

        static Message_ptr empty();
        zmq::message_t toZmqMsg() const;
        std::string toString() const;
        std::string returnAddr() const;
        int getPriority() const;

        std::string getFromHeader(const std::string &s) const;

        std::string getFromBody(const std::string &s) const;

        void setHeader(const std::string &key, const std::string &val);
        void setBody(const std::string &key, const std::string &val);
};


class Handler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Handler> {

    public:
        Handler(Message& parent) : msg(parent) {}
        Message& msg;
        std::shared_ptr<spdlog::logger> l;
        bool Null() {return true;}
        bool Bool(bool b) {return true;}
        bool Int(int i) {return true;}
        bool Uint(unsigned u) {return true;}
        bool Int64(int64_t i) {return true;}
        bool Uint64(uint64_t u) {return true;}
        bool Double(double d) {return true;}
        bool String(const char* str, rapidjson::SizeType length, bool copy) {
            if (msg.editingHeader)
                msg.setHeader(msg.currentKey, std::string(str, length));
            else
                msg.setBody(msg.currentKey, std::string(str, length));
            return true;
        }
        bool StartObject() {
            msg.editingHeader = msg.currentKey != std::string("body");
            return true;
        }
        bool Key(const char* str, rapidjson::SizeType length, bool copy) {
            msg.currentKey = str;
            return true;
        }
        bool EndObject(rapidjson::SizeType memberCount) {
            if (msg.editingHeader)
                msg.editingHeader = false;
            return true;
        }
        bool StartArray() {return true;}
        bool EndArray(rapidjson::SizeType elementCount) {return true;}
};

#endif
