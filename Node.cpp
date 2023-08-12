/*
 *  Created on: 28 apr. 2021
 *      Author: oveny
 */

#include "Node.h"

#include <Stream.h>

IComponent* Node::getComponent(uint8_t idx) {
  if (idx >= mSize) {
    return nullptr;
  }
  return mComponents[idx];
}

IComponent* Node::getComponentByEntityId(uint8_t entityId) {
  for (uint8_t i = 0; i < mSize; i++) {
    if (mComponents[i]->getEntityId() == entityId) {
      return mComponents[i];
    }
  }
  return nullptr;
}

size_t Node::print(Stream& stream) {
  uint8_t i;
  size_t n = 0;
  for (i = 0; i < mSize - 1; i++) {
    n += mComponents[i]->print(stream);
    n += stream.print(", ");
  }
  n += mComponents[i]->print(stream);
  n += stream.println();
  return n;
}
