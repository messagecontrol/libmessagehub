#include <gtest/gtest.h>

#include "messagehub/messagecontrol.h"
#include "messagehub/message.h"
#include "messagehub/manager.h"

class ManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<MessageControl> msgctlA, msgctlB;

    virtual void SetUp() {
        msgctlA = std::make_unique<MessageControl>("TEST_CONTROL_A", "localhost", 56789);
        msgctlB = std::make_unique<MessageControl>("TEST_CONTROL_B", "127.0.0.1", 56780);
        msgctlB->run();
        msgctlA->run();
    }

    // This is empty because the de-constructor will handle everything
    virtual void TearDown() {}

    std::shared_ptr<Message> make_message(std::shared_ptr<MessageControl> msgctl) {
        return Message::empty();
    }
};

TEST_F(ManagerTest, test_manager_initialization) {
    // Done in Fixture class
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

