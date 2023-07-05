/*
 *  Created on: 28 apr. 2021
 *      Author: oveny
 */

#include "Node.h"

Node::Node(IComponent* c0, IComponent* c1, IComponent* c2, IComponent* c3,
           IComponent* c4, IComponent* c5) {
  mEntities[0] = c0;
  mEntities[1] = c1;
  mEntities[2] = c2;
  mEntities[3] = c3;
  mEntities[4] = c4;
  mEntities[5] = c5;
}

uint8_t Node::getDiscoveryMsg(uint8_t* buffer, uint8_t idx) {
  return mEntities[idx]->getDiscoveryMsg(buffer);
}

uint8_t Node::getDiscoveryMsgByEntityId(uint8_t* buffer, uint8_t entityId) {
  IComponent* c = getComponentByEntityId(entityId);
  if (c) {
    return c->getDiscoveryMsg(buffer);
  }
  return 0;
}

uint8_t Node::getConfigItemValuesMsg(uint8_t* buffer, uint8_t entityId) {
  IComponent* c = getComponentByEntityId(entityId);
  if (c) {
    return c->getConfigItemValuesMsg(buffer);
  }
  return 0;
}

uint8_t Node::getValueMsg(uint8_t* buffer, uint8_t idx) {
  return mEntities[idx]->getValueMsg(buffer);
}

uint8_t Node::getValueMsgByEntityId(uint8_t* buffer, uint8_t entityId) {
  IComponent* c = getComponentByEntityId(entityId);
  if (c) {
    return c->getValueMsg(buffer);
  }
  return 0;
}

IComponent* Node::getComponent(uint8_t idx) {
  if (idx < getSize()) {
    return mEntities[idx];
  }
  return nullptr;
}

IComponent* Node::getComponentByEntityId(uint8_t entityId) {
  for (uint8_t i = 0; i < mEntities.maxSize; i++) {
    if (mEntities[i]->getEntityId() == entityId) {
      return mEntities[i];
    }
  }
  return nullptr;
}

void Node::print(Stream& stream) {
  uint8_t i;
  for (i = 0; i < mEntities.maxSize - 1; i++) {
    mEntities[i]->print(stream);
    stream.print(", ");
  }
  mEntities[i]->print(stream);
  stream.println();
}
