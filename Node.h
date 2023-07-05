/*
 *  Created on: 28 apr. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "BaseComponent.h"
#include "Util.h"

class Node {
 public:
  Node(BaseComponent* c0, BaseComponent* c1, BaseComponent* c2,
       BaseComponent* c3, BaseComponent* c4, BaseComponent* c5);

  uint8_t getDiscoveryMsg(uint8_t* buffer, uint8_t idx);
  uint8_t getDiscoveryMsgByEntityId(uint8_t* buffer, uint8_t entityId);

  uint8_t getConfigItemValuesMsg(uint8_t* buffer, uint8_t entityId);

  uint8_t getValueMsg(uint8_t* buffer, uint8_t idx);
  uint8_t getValueMsgByEntityId(uint8_t* buffer, uint8_t entityId);

  BaseComponent* getComponent(uint8_t idx);
  BaseComponent* getComponentByEntityId(uint8_t entityId);

  uint8_t getSize() const { return 6; }

  void print(Stream& stream);

 private:
  arrayHelper<BaseComponent*, 6> mEntities;
};
