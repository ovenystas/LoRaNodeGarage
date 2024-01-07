/*
 *  Created on: 28 apr. 2021
 *      Author: oveny
 */

#pragma once

#include <Printable.h>
#include <stdint.h>

#include "Component.h"

class Node : Printable {
 public:
  Node() = delete;

  Node(IComponent** components, uint8_t size)
      : mComponents{components}, mSize{size} {}

  IComponent* getComponent(uint8_t idx);
  IComponent* getComponentByEntityId(uint8_t entityId);

  uint8_t getSize() const { return mSize; }

  size_t printTo(Print& p) const final;

 private:
  const IComponent** mComponents;
  const uint8_t mSize;
};
