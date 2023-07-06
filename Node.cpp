/*
 *  Created on: 28 apr. 2021
 *      Author: oveny
 */

#include "Node.h"

IComponent* Node::getComponent(uint8_t idx) {
  if (idx < mSize) {
    return mComponents[idx];
  }
  return nullptr;
}

IComponent* Node::getComponentByEntityId(uint8_t entityId) {
  for (uint8_t i = 0; i < mSize; i++) {
    if (mComponents[i]->getEntityId() == entityId) {
      return mComponents[i];
    }
  }
  return nullptr;
}

void Node::print(Stream& stream) {
  uint8_t i;
  for (i = 0; i < mSize - 1; i++) {
    mComponents[i]->print(stream);
    stream.print(", ");
  }
  mComponents[i]->print(stream);
  stream.println();
}
