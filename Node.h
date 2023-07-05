/*
 *  Created on: 28 apr. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "Component.h"
#include "Util.h"

class Node {
 public:
  Node(IComponent* c0, IComponent* c1, IComponent* c2, IComponent* c3,
       IComponent* c4, IComponent* c5);

  uint8_t getDiscoveryMsg(uint8_t* buffer, uint8_t idx);
  uint8_t getDiscoveryMsgByEntityId(uint8_t* buffer, uint8_t entityId);

  uint8_t getConfigItemValuesMsg(uint8_t* buffer, uint8_t entityId);

  uint8_t getValueMsg(uint8_t* buffer, uint8_t idx);
  uint8_t getValueMsgByEntityId(uint8_t* buffer, uint8_t entityId);

  IComponent* getComponent(uint8_t idx);
  IComponent* getComponentByEntityId(uint8_t entityId);

  uint8_t getSize() const { return 6; }

  void print(Stream& stream);

 private:
  arrayHelper<IComponent*, 6> mEntities;
};
