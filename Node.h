/*
 *  Created on: 28 apr. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "components/Component.h"
#include "Util.h"

class Node {
public:
  Node(Component* c0, Component* c1, Component* c2,
      Component* c3, Component* c4/*, Component* c5*/);

  uint8_t getDiscoveryMsg(uint8_t* buffer, uint8_t idx);
  uint8_t getDiscoveryMsgByEntityId(uint8_t* buffer, uint8_t entityId);

  uint8_t getConfigItemValuesMsg(uint8_t* buffer, uint8_t entityId);

  uint8_t getValueMsg(uint8_t* buffer, uint8_t idx);
  uint8_t getValueMsgByEntityId(uint8_t* buffer, uint8_t entityId);

  Component* getComponent(uint8_t idx);
  Component* getComponentByEntityId(uint8_t entityId);

  uint8_t getSize() const {
    return 5;
  }

  void print(Stream& stream);

private:
  arrayHelper<Component*, 5> mEntities;
};

