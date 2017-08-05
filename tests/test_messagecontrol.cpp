#include <gtest/gtest.h>

#include "messagehub/messagecontrol.h"

class  MessageControlTest : public ::testing::Test {
protected:
    std::shared_ptr<MessageControl> msgctlA, msgctlB;

    virtual void SetUp() {
        msgctlA = std::make_shared<MessageControl>("TEST_CONTROL_A", "localhost", 56789);
        msgctlB = std::make_shared<MessageControl>("TEST_CONTROL_B", "127.0.0.1", 56780);
        msgctlB->run();
        msgctlA->run();
        msgctlA->connect(msgctlB->getAddr(), "TEST_CONTROL_B");
        // Allow time for handshake
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // This is empty because the de-constructor will handle everything
    virtual void TearDown() {}

    std::shared_ptr<Message> make_message(std::shared_ptr<MessageControl> msgctl) {
        Message_ptr m = Message::empty();
        m->setHeader("identity", msgctl->getIdentity());
        m->setHeader("returnAddr", msgctl->getAddr());
        return m;
    }
};

TEST_F(MessageControlTest, test_manager_initialization) {
    // Done in Fixture class
    // Yes, this takes care of many tests which goes against unit testing
}

TEST_F(MessageControlTest, test_queue_size) {
    Message_ptr msg = make_message(msgctlA);
    msg->setBody("text", "1");
    msgctlA->send(msg, msgctlB->getAddr());
    std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // Allow for message to be received
    EXPECT_EQ(msgctlB->getInQueueSize(), 1);

    for (int i = 0; i < 10; i++) {
        Message_ptr m = make_message(msgctlA);
        //m->setBody("text", std::to_string(i));
        msgctlA->send(m, msgctlB->getAddr());
        EXPECT_GT(msgctlA->getOutQueueSize(), 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    EXPECT_EQ(msgctlA->getOutQueueSize(), 0);
}


TEST_F(MessageControlTest, test_recv) {
    Message_ptr msg = make_message(msgctlA);
    msg->setBody("text", "123");
    msgctlA->send(msg, msgctlB->getAddr());

    while (msgctlB->getInQueueSize() == 0) {}
    Message_ptr m = msgctlB->recv();
    EXPECT_FALSE(m == nullptr);

    if (m != nullptr)
        EXPECT_EQ(m->getFromBody("text"), "123");
}

TEST_F(MessageControlTest, test_)

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

