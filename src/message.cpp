#include "messagehub/message.h"

JSONMessage::JSONMessage(const std::string &s) {
    parseString(s);
    editingHeader = true;
    currentKey = "[UNSET]";
}

JSONMessage::JSONMessage(zmq::message_t &zmsg) {
    std::string s = std::string(static_cast<char*>(zmsg.data()), zmsg.size());
    parseString(s);
    editingHeader = true;
    currentKey = "[UNSET]";
} 

std::shared_ptr<JSONMessage> JSONMessage::empty() {
    JSONMessage m = JSONMessage("{\"header\": { \"empty\":\"true\"}, \"body\":{}}");
    return std::make_shared<JSONMessage>(m);
}

void JSONMessage::parseString(const std::string & s) {
    rapidjson::Reader msg;
    Handler handler(*this);
    rapidjson::StringStream ss(s.c_str());
    msg.Parse(ss, handler);
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
        writer.String(elem.second.c_str());
    }
    writer.EndObject();
    writer.Key("body");
    writer.StartObject();
    for (std::pair<std::string, std::string> const elem : body) {
        writer.Key(elem.first.c_str());
        writer.String(elem.second.c_str());
    }
    writer.EndObject();
    writer.EndObject();
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
    // Not implemented yet
    return 1;
}

std::string JSONMessage::returnAddr() const {
    return getFromHeader("returnAddr");
}


JSONMessage::JSONMessage(const JSONMessage& msg) {
    header = msg.header;
    body = msg.body;
    currentKey = msg.currentKey;
    editingHeader = msg.editingHeader;
}

