#include <gtest/gtest.h>

#include "messagehub/messagecontrol.h"
#include "messagehub/message.h"
#include "messagehub/manager.h"

class  ManagerTest : public ::testing::Test {
protected:
    std::shared_ptr<MessageControl> msgctlA, msgctlB;
    std::shared_ptr<Manager> managerA, managerB;

    virtual void SetUp() {
        msgctlA = std::make_shared<MessageControl>("TEST_CONTROL_A", "localhost", 56789);
        msgctlB = std::make_shared<MessageControl>("TEST_CONTROL_B", "127.0.0.1", 56780);
        msgctlB->run();
        msgctlA->run();
        msgctlA->connect(msgctlB->getAddr(), "TEST_CONTROL_B");
        // Allow time for handshake
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        managerA = std::make_shared<Manager>(msgctlA);
        managerB = std::make_shared<Manager>(msgctlB);
        managerA->run();
        managerB->run();
    }


};


TEST_F(ManagerTest, test_initialization) {
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
