#include <gtest/gtest.h>

#include "messagehub/messagecontrol.h"
#include "messagehub/message.h"
#include "messagehub/manager.h"

class  MessageControlTest : public ::testing::Test {
protected:
    std::shared_ptr<MessageControl> msgctlA, msgctlB;

    virtual void SetUp() {
        msgctlA = std::make_shared<MessageControl>("TEST_CONTROL_A", "localhost", 56789);
        msgctlB = std::make_shared<MessageControl>("TEST_CONTROL_B", "127.0.0.1", 56780);
        msgctlB->run();
        msgctlA->run();
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
}

TEST_F(MessageControlTest, test_queue_size) {
    Message_ptr msg = make_message(msgctlA);
    msgctlA->send(msg, msgctlB->getAddr());
    std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // Allow for handshake
    EXPECT_EQ(msgctlB->getInQueueSize(), 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

