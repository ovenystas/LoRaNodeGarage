#include "Node.h"

const IComponent* Node::getComponent(uint8_t idx) {
  if (idx >= mSize) {
    return nullptr;
  }
  return mComponents[idx];
}

const IComponent* Node::getComponentByEntityId(uint8_t entityId) {
  for (uint8_t i = 0; i < mSize; i++) {
    if (mComponents[i]->getEntityId() == entityId) {
      return mComponents[i];
    }
  }
  return nullptr;
}

size_t Node::printTo(Print& p) const {
  uint8_t i;
  size_t n = 0;
  for (i = 0; i < mSize - 1; i++) {
    n += mComponents[i]->printTo(p);
    n += p.print(", ");
  }
  n += mComponents[i]->printTo(p);
  return n;
}
