#ifndef MESSAGE_H
#define MESSAGE_H

#ifndef RAPIDJSON_HAS_STDSTRING
//#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <string>
#include <sstream>
#include <memory>
#include <map>
// TODO: Use a logging module to log messages
#include <zmq.hpp>
#include "spdlog/spdlog.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "messagehub/exceptions.h"

class Handler;
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



class JSONMessage: public BaseMessage {

    friend class Handler;

    protected:
        void parseString(const std::string & s);
        std::map<std::string, std::string> header, body;
        std::string currentKey;
        bool editingHeader;
        void set(const std::string &val);

    public:
        JSONMessage(const std::string &s);
        JSONMessage(zmq::message_t &zmsg);
        JSONMessage(const JSONMessage &msg);

        static std::shared_ptr<JSONMessage> empty();
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
        Handler(JSONMessage& parent) : msg(parent) {l = spdlog::get("MessageControl");}
        JSONMessage& msg;
        std::shared_ptr<spdlog::logger> l;
        bool Null() {return true;}
        bool Bool(bool b) {return true;}
        bool Int(int i) {return true;}
        bool Uint(unsigned u) {return true;}
        bool Int64(int64_t i) {return true;}
        bool Uint64(uint64_t u) {return true;}
        bool Double(double d) {return true;}
        bool String(const char* str, rapidjson::SizeType length, bool copy) {
            if (msg.editingHeader) {
                l->trace("Inserting {} and {} to {}", msg.currentKey, std::string(str,length), "header");
                msg.setHeader(msg.currentKey, std::string(str, length));
            } else {
                l->trace("Inserting {} and {} to {}", msg.currentKey, std::string(str, length), "body");
                msg.setBody(msg.currentKey, std::string(str, length));
            }
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
