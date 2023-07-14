#include "Node.h"

#include <gtest/gtest.h>

#include "Component.h"
#include "mocks/Print.h"
#include "mocks/Stream.h"

class ComponentChild : public IComponent {
 public:
  explicit ComponentChild(uint8_t entityId) : mEntityId{entityId} {}

  void callService(uint8_t service) { (void)service; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) {
    (void)buffer;
    return 0;
  }

  uint8_t getDiscoveryMsg(uint8_t* buffer) {
    (void)buffer;
    return 0;
  }

  uint8_t getEntityId() const { return mEntityId; }

  uint8_t getValueMsg(uint8_t* buffer) {
    (void)buffer;
    return 0;
  }

  void print(Stream& stream) { stream.print(mEntityId); }

  void print(Stream& stream, uint8_t service) {
    (void)stream;
    (void)service;
  }

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) {
    (void)numberOfConfigs;
    (void)buffer;
    return false;
  }

  void setReported() {}

  bool update() { return false; }

 private:
  const uint8_t mEntityId;
};

class Node_test : public ::testing::Test {
 protected:
  ComponentChild c0 = ComponentChild(10);
  ComponentChild c1 = ComponentChild(11);
  IComponent* c[2] = {&c0, &c1};
  Node n = Node(c, 2);
};

TEST_F(Node_test, getSize) { EXPECT_EQ(n.getSize(), 2); }

TEST_F(Node_test, getComponent) {
  EXPECT_EQ(n.getComponent(0), &c0);
  EXPECT_EQ(n.getComponent(1), &c1);
}

TEST_F(Node_test, getComponentByEntityId) {
  EXPECT_EQ(n.getComponentByEntityId(12), nullptr);
  EXPECT_EQ(n.getComponentByEntityId(11), &c1);
  EXPECT_EQ(n.getComponentByEntityId(10), &c0);
}

// TEST_F(Node_test, print) {
//   StreamMock* pStreamMock = new StreamMock();
//   EXPECT_CALL(print())
// }
