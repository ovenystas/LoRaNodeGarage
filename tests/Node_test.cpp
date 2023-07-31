#include "Node.h"

#include <gtest/gtest.h>

#include "Component.h"
#include "mocks/BufferSerial.h"
#include "mocks/Print.h"
#include "mocks/Stream.h"

class ComponentChild : public IComponent {
 public:
  explicit ComponentChild(uint8_t entityId) : mEntityId{entityId} {}

  void callService(uint8_t service) { (void)service; }

  uint8_t getConfigItemValues(ConfigItemValueT* items, uint8_t length) const {
    (void)items;
    (void)length;
    return 0;
  }

  void getDiscoveryItem(DiscoveryItemT* item) const { (void)item; }

  uint8_t getEntityId() const { return mEntityId; }

  void getValueItem(ValueItemT* item) const { (void)item; }

  bool isReportDue() const { return false; }

  size_t print(Stream& stream) const { return stream.print(mEntityId); }

  size_t print(Stream& stream, uint8_t service) const {
    (void)stream;
    (void)service;
    return 0;
  }

  bool setConfigItemValues(const ConfigItemValueT* items, uint8_t length) {
    (void)items;
    (void)length;
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
  IComponent* components[2] = {&c0, &c1};
  Node n = Node(components, 2);
};

class NodePrint_test : public Node_test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    strBuf[0] = '\0';
  }

  void TearDown() override { delete pSerial; }

  void bufSerReadStr() {
    size_t i = 0;
    while (pSerial->available()) {
      int c = pSerial->read();
      if (c < 0) {
        break;
      }
      strBuf[i++] = static_cast<char>(c);
    }
    strBuf[i] = '\0';
  }

  char strBuf[256];
  BufferSerial* pSerial;
};

TEST_F(Node_test, getSize) { EXPECT_EQ(n.getSize(), 2); }

TEST_F(Node_test, getComponent) {
  EXPECT_EQ(n.getComponent(0), &c0);
  EXPECT_EQ(n.getComponent(1), &c1);
  EXPECT_EQ(n.getComponent(2), nullptr);
}

TEST_F(Node_test, getComponentByEntityId) {
  EXPECT_EQ(n.getComponentByEntityId(12), nullptr);
  EXPECT_EQ(n.getComponentByEntityId(11), &c1);
  EXPECT_EQ(n.getComponentByEntityId(10), &c0);
}

TEST_F(NodePrint_test, print) {
  const char* expectedStr = "10, 11\r\n";

  EXPECT_EQ(n.print(*pSerial), strlen(expectedStr));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectedStr);
}
