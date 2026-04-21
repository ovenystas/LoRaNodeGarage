#include "Device.h"

IComponent* Device::getComponent(uint8_t idx) {
  if (idx >= mSize) {
    return nullptr;
  }
  return mComponents[idx];
}

IComponent* Device::getComponentByEntityId(uint8_t entityId) {
  for (uint8_t i = 0; i < mSize; i++) {
    if (mComponents[i]->getEntityId() == entityId) {
      return mComponents[i];
    }
    IComponent* c = mComponents[i]->getComponentByEntityId(entityId);
    if (c) {
      return c;
    }
  }
  return nullptr;
}

size_t Device::printTo(Print& p) const {
  size_t n = 0;

  for (uint8_t i = 0; i < mSize; i++) {
    if (i > 0) {
      n += p.print(", ");
    }
    n += p.print(mComponents[i]->getEntityId());
    n += p.print(':');
    n += mComponents[i]->printTo(p);
  }

  n += p.println();

  return n;
}
