#include "messagehub/message.h"

Message::Message(const std::string &s) {
    parseString(s);
    editingHeader = true;
    currentKey = "[UNSET]";
}

Message::Message(zmq::message_t &zmsg) {
    std::string s = std::string(static_cast<char*>(zmsg.data()), zmsg.size());
    parseString(s);
    editingHeader = true;
    currentKey = "[UNSET]";
}

Message_ptr Message::empty() {
    Message m = Message(R"({"header": {}, "body":{}})");
    return std::make_shared<Message>(m);
}

void Message::parseString(const std::string & s) {
    rapidjson::Reader msg;
    Handler handler(*this);
    rapidjson::StringStream ss(s.c_str());
    msg.Parse(ss, handler);
}

zmq::message_t Message::toZmqMsg() const {
    const std::string s = toString();
    zmq::message_t msg(s.size());
    std::copy(s.begin(), s.end(), static_cast<char *>(msg.data()));
    return msg;
}

std::string Message::toString() const {
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

std::string Message::getFromHeader(const std::string &key) const {
    if (header.find(key) != header.end())
        return header.at(key);
    else
        return "";
}

std::string Message::getFromBody(const std::string &key) const {
    if (body.find(key) != body.end())
        return body.at(key);
    else
        return "";
}

void Message::setHeader(const std::string &key, const std::string &val) {
    if (header.find(key) == header.end()) {
        header.insert(std::make_pair(key, val));
    } else {
        header[key] = val;
    }
}

void Message::setBody(const std::string &key, const std::string &val) {
    if (body.find(key) == header.end()) {
        body.insert(std::make_pair(key, val));
    } else {
        body[key] = val;
    }
}

int Message::getPriority() const {
    // Not implemented yet
    return 1;
}

std::string Message::returnAddr() const {
    return getFromHeader("returnAddr");
}


Message::Message(const Message& msg) {
    header = msg.header;
    body = msg.body;
    currentKey = msg.currentKey;
    editingHeader = msg.editingHeader;
}

