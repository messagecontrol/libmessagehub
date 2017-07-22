#include <string>
#include <gtest/gtest.h>

#include "messagehub/message.h"

class MessageTest : public ::testing::Test {
    protected:
        std::shared_ptr<Message> msg;
        virtual void SetUp() {
           msg = Message::empty();
        } 
        virtual void TearDown() {}
};

TEST_F(MessageTest, test_add_header) {
    msg->setHeader("test_header", "TEST");
    EXPECT_EQ(msg->getFromHeader("test_header"), std::string("TEST"));
}

TEST_F(MessageTest, test_add_body) {
    msg->setBody("test_body", "TEST");
    EXPECT_EQ(msg->getFromBody("test_body"), std::string("TEST"));
}

TEST_F(MessageTest, test_not_found) {
    EXPECT_EQ(msg->getFromHeader("non_existing_key"), std::string(""));
    EXPECT_EQ(msg->getFromBody("non_existing_key"), std::string(""));
}

TEST_F(MessageTest, test_overwrite) {
    msg->setHeader("someHead", "first_val");
    msg->setHeader("someHead", "second_val");
    EXPECT_EQ(msg->getFromHeader("someHead"), std::string("second_val"));
}

TEST_F(MessageTest, test_parse) {
    EXPECT_NO_THROW({
        const std::string s = R"({"header":{}, "body":{}})";
        Message m = Message(s);
                    });
    EXPECT_NO_THROW({
        const std::string s = "{\"header\":{\"test\": \"testing\"},\"body\":{}}";
        Message m = Message(s);
                    });
    EXPECT_NO_THROW({
        const std::string s = R"({"header":{}, "body":{"hello":"world"}})";
        Message m = Message(s);
                    });
    EXPECT_NO_THROW({
        const std::string s = R"({"header":{"test": "testing"},"body":{"hello":"world"}})";
        Message m = Message(s);
                    });
}

TEST_F(MessageTest, test_initialize_from_zmsg) {
    msg->setHeader("testing", "TEST");
    msg->setBody("Testing", "TEST");
    zmq::message_t zmsg = msg->toZmqMsg();
    Message m = Message(zmsg);
    EXPECT_EQ(m.toString(), std::string(static_cast<char*>(zmsg.data()), zmsg.size()));
}

TEST_F(MessageTest, test_to_zmq_message) {
    zmq::message_t zmsg = msg->toZmqMsg();
    EXPECT_EQ(std::string(static_cast<char*>(zmsg.data()), zmsg.size()),
              std::string("{\"header\":{},\"body\":{}}"));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

