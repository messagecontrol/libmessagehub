#include "messagehub/message.h"

JSONMessage::JSONMessage(const std::string &s): handler(std::make_shared<Handler>(std::shared_ptr<JSONMessage>(this))) {
    parseString(s);
}

JSONMessage::JSONMessage(zmq::message_t &zmsg): handler(std::make_shared<Handler>(std::shared_ptr<JSONMessage>(this))) {
    std::string s = std::string(static_cast<char*>(zmsg.data()), zmsg.size());
    parseString(s);
} 

JSONMessage JSONMessage::empty() {
    return JSONMessage("{\"header\": {}, \"body\":{}}");
}

void JSONMessage::parseString(const std::string & s) {
    rapidjson::Document msg;
    msg.Parse(s);
    // Some message requirements
    if (!msg.IsObject()) 
        throw MessageFormatException("Message JSON is not an object");
    if (!msg.HasMember("header"))
        throw MessageFormatException("Message is missing header", s);  
    if (!msg["header"].IsObject())
        throw MessageFormatException("Message's header is not an object", s);
    if (!msg.HasMember("body"))
        throw MessageFormatException("Message is missing an body", s);
    if (!msg["body"].IsObject())
        throw MessageFormatException("Message's body is not an object", s); 
    msg.Accept(*handler);
}

zmq::message_t JSONMessage::toZmqMsg() const {
    const std::string s = toString();
    zmq::message_t msg(s.size());
    std::copy(s.begin(), s.end(), static_cast<char *>(msg.data()));
    return msg;
}

std::string JSONMessage::toString() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.Key("header");
    writer.StartObject();
    for (std::pair<std::string, std::string> const elem : header) {
        writer.Key(elem.first.c_str());
        writer.String(elem.second);
    }
    writer.EndObject();
    writer.Key("body");
    writer.StartObject();
    for (std::pair<std::string, std::string> const elem : body) {
        writer.Key(elem.first.c_str());
        writer.String(elem.second);
    }
    writer.EndObject();
    writer.EndObject();
    std::cout << buffer.GetString() << "\n";
    return buffer.GetString();
}

std::string JSONMessage::getFromHeader(const std::string &key) const {
    if (header.find(key) != header.end())
        return header.at(key);
    else 
        return "";
}

std::string JSONMessage::getFromBody(const std::string &key) const {
    if (body.find(key) != body.end())
        return body.at(key);
    else
        return "";
}

void JSONMessage::setHeader(const std::string &key, const std::string &val) {
    if (header.find(key) == header.end()) {
        header.insert(std::make_pair(key, val));
    } else {
        header[key] = val;
    }
}

void JSONMessage::setBody(const std::string &key, const std::string &val) {
    if (body.find(key) == header.end()) {
        body.insert(std::make_pair(key, val));
    } else {
        body[key] = val;
    }
}

int JSONMessage::getPriority() const {
    return 1;
}

std::string JSONMessage::returnAddr() const {
    return getFromHeader("returnAddr");
}


JSONMessage::JSONMessage(const JSONMessage& msg) {
    std::cout << "COPYING message\n";
    header = msg.header;
    body = msg.body;
    handler = msg.handler;
    currentKey = msg.currentKey;
    editingHeader = msg.editingHeader;
}

